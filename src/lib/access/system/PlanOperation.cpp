// Copyright (c) 2012 Hasso-Plattner-Institut fuer Softwaresystemtechnik GmbH. All rights reserved.
#include "access/system/PlanOperation.h"

#include <algorithm>
#include <thread>

#include "access/system/ResponseTask.h"
#include "helper/epoch.h"
#include "helper/PapiTracer.h"
#include "io/StorageManager.h"
#include "storage/AbstractResource.h"
#include "storage/AbstractHashTable.h"
#include "storage/AbstractTable.h"
#include "storage/TableRangeView.h"

#include "boost/lexical_cast.hpp"
#include "log4cxx/logger.h"

namespace { auto logger = log4cxx::Logger::getLogger("access.plan.PlanOperation"); }

namespace hyrise { namespace access {

PlanOperation::~PlanOperation() = default;

void PlanOperation::addResult(storage::c_aresource_ptr_t result) {
  output.addResource(result);
}

const storage::c_atable_ptr_t PlanOperation::getInputTable(size_t index) const {
  return input.getTable(index);
}

static const storage::atable_ptr_t empty_result;

const storage::c_atable_ptr_t PlanOperation::getResultTable(size_t index) const {
  if (output.numberOfTables())
    return output.getTable(index);
  else
     return empty_result;
}

storage::c_ahashtable_ptr_t PlanOperation::getInputHashTable(size_t index) const {
  return input.getHashTable(index);
}

storage::c_ahashtable_ptr_t PlanOperation::getResultHashTable(size_t index) const {
  return output.getHashTable(index);
}

bool PlanOperation::allDependenciesSuccessful() {
  for (size_t i = 0; i < _dependencies.size(); ++i) {
    if (std::dynamic_pointer_cast<OutputTask>(_dependencies[i])->getState() == OpFail) return false;
  }
  return true;
}

/*
std::string PlanOperation::getDependencyErrorMessages() {
  std::string result;

  std::shared_ptr<OutputTask> task;
  for (size_t i = 0; i < _dependencies.size(); ++i) {
    task = std::dynamic_pointer_cast<OutputTask>(_dependencies[i]);
    if (task->getState() == OpFail) result += task->getErrorMessage() + "\n";
  }

  return result;
  }*/

void PlanOperation::addField(field_t field) {
  _indexed_field_definition.push_back(field);
}

void PlanOperation::addNamedField(const field_name_t& field) {
  _named_field_definition.push_back(field);
}

void PlanOperation::addField(const Json::Value &field) {
  if (field.isNumeric()) {
    addField(field.asUInt());
  } else if (field.isString()) {
    addNamedField(field.asString());
  } else throw std::runtime_error("Can't parse field name, neither numeric nor std::string");
}

void PlanOperation::computeDeferredIndexes() {
  _field_definition = _indexed_field_definition;
  if (!_named_field_definition.empty()) {
    assert(_field_definition.empty());
    const auto& table = input.getTable(0);
    if ((_named_field_definition.size() == 1) && (_named_field_definition[0] == "*")) {
      _field_definition.resize(table->columnCount());
      std::iota(std::begin(_field_definition), std::end(_field_definition), 0);
    } else {
      for (size_t i = 0; i < _named_field_definition.size(); ++i) {
        _field_definition.push_back(table->numberOfColumn(_named_field_definition[i]));
      }
    }
  }
  assert(_field_definition.size() >= (_indexed_field_definition.size() + _named_field_definition.size()));
}

void PlanOperation::setupPlanOperation() {
  computeDeferredIndexes();
}


void PlanOperation::refreshInput() {
  size_t numberOfDependencies = _dependencies.size();
  for (size_t i = 0; i < numberOfDependencies; ++i) {
    const auto& dependency = std::dynamic_pointer_cast<PlanOperation>(_dependencies[i]);
    input.mergeWith(dependency->output);
  }
}

void PlanOperation::setErrorMessage(const std::string& message) {
  LOG4CXX_INFO(logger, this << " " << planOperationName() << " sets error message: " << message);
  getResponseTask()->addErrorMessage(_operatorId + ":  " + message);
}

void PlanOperation::operator()() noexcept {
  if (allDependenciesSuccessful()) {
    try {
      LOG4CXX_DEBUG(logger, "Executing " << vname() << "(" << _operatorId << ")");
      execute();
      return;
    } catch (const std::exception &ex) {
      setErrorMessage(ex.what());
    } catch (...) {
      setErrorMessage("Unknown error");
    }
  }
  setState(OpFail);
}

const PlanOperation * PlanOperation::execute() {
  epoch_t startTime = get_epoch_nanoseconds();

  refreshInput();

  setupPlanOperation();

  // PapiTracer pt;
  // if (!_papi_disabled) {
  //   pt.addEvent("PAPI_TOT_CYC");
  //   pt.addEvent(getEvent());
  //   pt.start();
  // }

  executePlanOperation();
  
  // if (!_papi_disabled)
  //   pt.stop();

  teardownPlanOperation();

  if (_performance_attr != nullptr) {
    epoch_t endTime = get_epoch_nanoseconds();
    std::string threadId = boost::lexical_cast<std::string>(std::this_thread::get_id());

    long long papi_cycles = 0;
    long long papi_event = 0;

    // if (!_papi_disabled) {
    //   papi_cycles = pt.value("PAPI_TOT_CYC");
    //   papi_event = pt.value(getEvent());
    // }
    
    *_performance_attr = (performance_attributes_t) {
      papi_cycles, papi_event, getEvent() , planOperationName(), _operatorId, startTime, endTime, threadId
    };
  }

  setState(OpSuccess);
  return this;
}

void PlanOperation::setLimit(uint64_t l) {
  _limit = l;
}

void PlanOperation::setProducesPositions(bool p) {
  producesPositions = p;
}

void PlanOperation::setTXContext(tx::TXContext ctx) {
  _txContext = ctx;
}

void PlanOperation::addInput(storage::c_aresource_ptr_t t) {
  input.addResource(t);
}

void PlanOperation::setPlanId(std::string i) {
  _planId = i;
}
void PlanOperation::setOperatorId(std::string i) {
  _operatorId = i;
}

const std::string& PlanOperation::planOperationName() const {
  return _planOperationName;
}
void PlanOperation::setPlanOperationName(const std::string& name) {
  _planOperationName = name;
}

const std::string PlanOperation::vname() {
  return planOperationName();
}

void PlanOperation::setResponseTask(const std::shared_ptr<access::ResponseTask>& responseTask) {
  _responseTask = responseTask;
}

std::shared_ptr<access::ResponseTask> PlanOperation::getResponseTask() const {
  return _responseTask.lock();
}

void PlanOperation::disablePapiTrace() {
  _papi_disabled = true;
}


}}
