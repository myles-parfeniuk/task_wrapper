#pragma once
//esp-idf includes
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "esp_err.h"
#include "esp_log.h"
//standard library includes
#include <functional>
#include "TaskConfigDefs.hpp"



/**
*
* @brief Encapsulates a task and implements methods to interact with it.  
*
* Creates a task object which is passed a task function. 
* The task can then be started, suspended, resumed, or deleted through other methods within this class.
*
* @author Myles Parfeniuk
*
*/
template <class T = void *>
class TaskWrapper {    

public:


  /**
  * @brief Construct a TaskWrapper object with a void input parameter task.
  * 
  * Construct a TaskWrapper according to input parameters.
  * The task function is the only mandatory parameter. 
  * 
  * Should be declared with the following syntax if outside of class: 
  * 
  * void task_function();
  * TaskWrapper<> my_task(&task_function);
  * 
  * Should be declared with the following syntax if inside of class (where task_function is class method): 
  * void task_function();
  * TaskWrapper<MyClassName> my_task(std::bind(&MyClassName::task_function, this));
  * 
  * @param task task function, must take the form "void my_task(void)"
  * @param task_config configuration settings for task, (optional, see default_config)
  * 
  * @return void, nothing to return
  */
  TaskWrapper(std::function<void(void)> task, const task_config_t task_config = default_config):
    task([task](T*) { //initialized as lambda such that void(void) function can be saved to void(T *) member
        task();
    }),
    task_input(nullptr),
    task_hdl(NULL),
    task_config(task_config),
    task_active(false)
  {
  }
  

  /**
  * @brief Construct a TaskWrapper object with a non-void input parameter task.
  * 
  * Construct a TaskWrapper according to input parameters. 
  * The task function and task input are the only mandatory parameters
  * 
  * Should be declared with the following syntax:
  * 
  * void task_function(T *);
  * T my_data = some_value; 
  * TaskWrapper<T> my_task(&task_function, &input_data);
  * 
  * 
  * @param task task function, must take the form "void my_task(T* input_param)"
  * @param task_input a pointer to the task function input parameter
  * @param task_config configuration settings for task, (optional, see default_config)
  * 
  * @return void, nothing to return
  */
  TaskWrapper(std::function<void(T*)> task, T* task_input, const task_config_t task_config = default_config):
      task(task),
      task_input(task_input),
      task_hdl(NULL),
      task_config(task_config),
      task_active(false)
  {
  }



  

   /**
  * @brief Start the task function.
  * 
  * Calls xTaskCreatePinnedToCore() from the freeRTOS api and executes the task_function member from the context of the newly created task. 
  * 
  * 
  * @return True if task successfully started, false if it is already running
  */
  bool start_task();

   /**
  * @brief Deletes task.
  * 
  * Calls vTaskDelete() from the freeRTOS api.
  * 
  * @return True if task successfully deleted, false if task is not started 
  */
  bool delete_task();

   /**
  * @brief Suspends task.
  * 
  * Calls vTaskSuspend() from the freeRTOS api.
  * 
  * @return True if task successfully suspended, false if task is not started 
  */
  bool suspend_task();

   /**
  * @brief Resumes task.
  * 
  * Calls vTaskResume() from the freeRTOS api.
  * 
  * @return True if task successfully resumed, false if task is not started 
  */
  bool resume_task();

  /**
  * @brief Resumes task from context of ISR.
  * 
  * This method should be called instead of resume_task() if suspending a task from the context of an ISR is desired. 
  * 
  * @return True if task successfully resumed, false if task is not started 
  */
  bool resume_task_from_isr();

  /**
  * @brief Sets task stack size. 
  * 
  * Sets the stack size of the task created when start_task() is called.
  * If the task is currently running this function will delete it and re-create it.
  * 
  * @return void, nothing to return
  */
  void set_stack_size(uint32_t new_stack_size);

  /**
  * @brief Sets task priority.
  * 
  * Sets the priority of the task created when start_task() is called.
  * If the task is currently running this function will delete it and re-create it.
  * 
  * @return void, nothing to return
  */
  void set_priority(UBaseType_t priority);

  /**
  * @brief Returns minimum free stack space in bytes.
  * 
  * Calls uxTaskGetStackHighWaterMark from the freeRTOS api. 
  * 
  * @return the free stack space of the task (in bytes)
  */
  uint16_t get_high_water_mark();

  /**
  * @brief Returns the task handle.
  * 
  * Returns the task handle of the task encapsulated by this TaskWrapper object.
  * May be used if it is desired to call functions from the freeRTOS api directly. 
  * 
  * @return the free stack space of the task (in bytes)
  */
  TaskHandle_t &get_task_handle();


  bool is_active(); 

private:

 /**
 * @brief Launches task.
 * 
 * This function is used to get around the fact xTaskCreatePinnedToCore() from the freertos api requires a static task function.
 * To prevent having to write the callback task from the context of a static function, this serves as a trampoline to launch the task.
 * 
 * @return void, nothing to return
 */
  static void task_trampoline(void *arg);

  std::function<void(T *)> task; ///<task function 
  T *task_input; ///<the input parameter that will be passed into task function when it is started
  TaskHandle_t task_hdl; ///<task handle of task function
  task_config_t task_config; ///<task config settings
  bool task_active; ///< whether or not the task is currently running, false if not started or suspended
  static const constexpr char* TAG = "TaskWrapper"; ///<class tag, used in debug logs
};

template <typename T>
void TaskWrapper<T>::task_trampoline(void *arg)
{
  //cast void pointer to TaskWrapper pointer
  TaskWrapper* local_task = static_cast<TaskWrapper*>(arg);
  //call task function and pass user set input parameter
  local_task->task(local_task->task_input);
  //delete task if it completes  
  local_task->delete_task();
}

template <typename T>
bool TaskWrapper<T>::start_task()
{
  //if task is not started
  if (task_hdl == NULL) 
  {
    //create task
    xTaskCreatePinnedToCore(&task_trampoline, "task", task_config.stack_size, this, task_config.priority, &task_hdl, task_config.core_id);
    task_active = true; 
    return true; 
  }
  
  if (task_config.logging_en)
    ESP_LOGE(TAG, "%s -> task already started", task_config.name);

  return false;
}

template <typename T>
bool TaskWrapper<T>::delete_task()
{
  //if task is started
  if (&task_hdl != NULL) {
    //delete task
    task_active = false;
    vTaskDelete(task_hdl); 
    task_hdl = NULL;
    return true;
  }

  if (task_config.logging_en)
    ESP_LOGE(TAG, "%s -> task not started", task_config.name);

  return false;
}

template <typename T>
bool TaskWrapper<T>::suspend_task()
{
  //if task is started
  if (&task_hdl != NULL) {
    //suspend task
    task_active = false; 
    vTaskSuspend(task_hdl);
    return true;
  }

  if (task_config.logging_en)
    ESP_LOGE(TAG, "%s -> task not started", task_config.name);

  return false;
}

template <typename T>
bool TaskWrapper<T>::resume_task()
{
  //if task is started
  if (&task_hdl != NULL) {
    //resume task
    vTaskResume(task_hdl);
    task_active = true; 
    return true;
  }

  if (task_config.logging_en)
    ESP_LOGE(TAG, "%s -> task not started", task_config.name);

  return false;
}

template <typename T>
bool TaskWrapper<T>::resume_task_from_isr()
{
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  //if task is started
  if (&task_hdl != NULL) {
    //resume task
    xHigherPriorityTaskWoken = xTaskResumeFromISR(task_hdl);
    if(xHigherPriorityTaskWoken == pdTRUE)
    {
        //Perform a context switch so this interrupt returns directly to the unblocked task
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    task_active = true; 
    return true;
  }

  if (task_config.logging_en)
    ESP_LOGE(TAG, "%s -> task not started", task_config.name);

  return false;

}

template <typename T>
uint16_t TaskWrapper<T>::get_high_water_mark()
{
  //if task started
  if (task_hdl != NULL)
    //get minimum free stack size
    return (uint16_t)uxTaskGetStackHighWaterMark(task_hdl);

  if (task_config.logging_en)
    ESP_LOGE(TAG, "%s -> task not started", task_config.name);

  return false;
}

template <typename T>
void TaskWrapper<T>::set_stack_size(uint32_t new_stack_size)
{
  //set new stack size
  task_config.stack_size = new_stack_size; 

  //if task is started
  if (task_hdl != NULL) {
    //re-create task such that new stack size takes effect
    delete_task();
    xTaskCreatePinnedToCore(&task_trampoline, "task", task_config.stack_size, this, task_config.priority, &task_hdl, task_config.core_id);
  }
}

template <typename T>
void TaskWrapper<T>:: set_priority(UBaseType_t new_priority)
{
  //set new priority
  task_config.priority = new_priority; 

  //if task is started
  if (task_hdl != NULL) {
    //re-create task such that new priority takes effect
    delete_task();
    xTaskCreatePinnedToCore(&task_trampoline, "task", task_config.stack_size, this, task_config.priority, &task_hdl, task_config.core_id);
  }

}

template <typename T>
TaskHandle_t &TaskWrapper<T>::get_task_handle()
{
  return task_hdl;
}

template <typename T>
bool TaskWrapper<T>::is_active()
{
  return task_active;
}