// Copyright (c) 2012 Hasso-Plattner-Institut fuer Softwaresystemtechnik GmbH. All rights reserved.
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <hwloc.h>
#include <signal.h>

#include "handler.h"

#include "log4cxx/logger.h"
#include "log4cxx/basicconfigurator.h"
#include "log4cxx/propertyconfigurator.h"
#include "log4cxx/helpers/exception.h"

#include <boost/program_options.hpp>

#include "helper/HwlocHelper.h"
#include "net/AsyncConnection.h"
#include "io/StorageManager.h"
#include "taskscheduler/SharedScheduler.h"

namespace po = boost::program_options;
using namespace hyrise;
using namespace log4cxx;
using namespace log4cxx::helpers;

namespace  {
  LoggerPtr logger(Logger::getLogger("hyrise"));
}




void bindToNode(int node) {
  hwloc_topology_t topology = getHWTopology();
  hwloc_cpuset_t cpuset;
  hwloc_obj_t obj;

  // The actual core
  obj = hwloc_get_obj_by_type(topology, HWLOC_OBJ_CORE, node);
  cpuset = hwloc_bitmap_dup(obj->cpuset);
  hwloc_bitmap_singlify(cpuset);

  // bind
  if (hwloc_set_cpubind(topology, cpuset, HWLOC_CPUBIND_STRICT | HWLOC_CPUBIND_NOMEMBIND | HWLOC_CPUBIND_PROCESS)) {
    char *str;
    int error = errno;
    hwloc_bitmap_asprintf(&str, obj->cpuset);
    printf("Couldn't bind to cpuset %s: %s\n", str, strerror(error));
    free(str);
    throw std::runtime_error(strerror(error));
  }

  // free duplicated cpuset
  hwloc_bitmap_free(cpuset);

  obj = hwloc_get_obj_by_type(topology, HWLOC_OBJ_MACHINE, node);
  if (hwloc_set_membind_nodeset(topology, obj->nodeset, HWLOC_MEMBIND_INTERLEAVE, HWLOC_MEMBIND_STRICT | HWLOC_MEMBIND_THREAD)) {
    char *str;
    int error = errno;
    hwloc_bitmap_asprintf(&str, obj->nodeset);
    fprintf(stderr, "Couldn't membind to nodeset  %s: %s\n", str, strerror(error));
    fprintf(stderr, "Continuing as normal, however, no guarantees\n");
    free(str);
  }
}


int main(int argc, char *argv[]) {
  int worker_threads = 0;
  std::string logPropertyFile;
  std::string scheduler_name;
  std::string content_file;
  size_t iterations = 0;

  // Program Options
  po::options_description desc("Allowed Parameters");
  desc.add_options()("help", "Shows this help message")
  ("file,f", po::value<std::string>(&content_file), "File To Execute")
  ("iter,i", po::value<size_t>(&iterations), "Iterations")
  ("logdef,l", po::value<std::string>(&logPropertyFile)->default_value("build/log.properties"), "Log4CXX Log Properties File")
  ("scheduler,s", po::value<std::string>(&scheduler_name)->default_value("ThreadPerTaskScheduler"), "Name of the scheduler to use")
  ("threads,t", po::value<int>(&worker_threads)->default_value(getNumberOfCoresOnSystem()), "Number of worker threads for scheduler (only relevant for scheduler with fixed number of threads)");
  po::variables_map vm;

  try {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
  } catch(po::error &e) {
    std::cerr << e.what() << std::endl;
    return EXIT_SUCCESS;
  }

  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return EXIT_SUCCESS;
  }


  //Bind the program to the first NUMA node for schedulers that have core bound threads
  if((scheduler_name == "CoreBoundQueuesScheduler") || (scheduler_name == "CoreBoundQueuesScheduler") ||  (scheduler_name == "WSCoreBoundQueuesScheduler") || (scheduler_name == "WSCoreBoundPriorityQueuesScheduler"))
    bindToNode(0);

  // Log File Configuration
  PropertyConfigurator::configure(logPropertyFile);

#ifndef PRODUCTION
  LOG4CXX_WARN(logger, "compiled with development settings, expect substantially lower and non-representative performance");
#endif


  HyriseHandler handler(scheduler_name, worker_threads);
  handler.init();

  // Get the file from path
  std::string content = HyriseHandler::loadFromFile(content_file);
  

  size_t x = 0;
  for(size_t i=0; i < iterations; ++i) {
    x += handler.execute(content).size();
  }
  return x;
}
