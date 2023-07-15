static const constexpr uint32_t default_stack_size = 4096U; ///<default task stack size
static const constexpr UBaseType_t default_priority = 8U; ///<default priority
static const constexpr BaseType_t default_core_id = 0U; ///<default core ID


struct task_config_t
{
  uint32_t stack_size = default_stack_size; ///<stack size of task
  UBaseType_t priority = default_priority;; ///<priority of the task
  BaseType_t core_id = default_core_id; ///<index number of the CPU task should be pinned to
  bool logging_en = false;  ///<whether or not logging is enabled, prints debug statements to terminal
  const char *name = "unamed"; ///<name of TaskWrapper object, used in log statements
};


static constexpr task_config_t default_config;