<a name="readme-top"></a>
![image](TaskWrapper_banner.png)
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li><a href="#about">About</a></li>
    <li><a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#adding-to-project">Adding to Project</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a>
      <ul>
        <li><a href="#quick-start">Quick Start</a></li>
        <ul>
          <li><a href="#instantiating-a-taskwrapper-object">Instantiating a TaskWrapper Object</a></li>
          <li><a href="#task-configuration">Task Configuration</a></li>
          <li><a href="#interacting-with-tasks">Interacting with Tasks</a></li>
        </ul>
        <li><a href="#examples">Examples</a></li>
      </ul>
    </li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>

<!-- ABOUT -->
## About

TaskWrapper is a C++ based component written for esp-idf version 5.0, intended to simplify the creation and management of tasks within a C++ environment. 
It allows for the creation of TaskWrapper objects which are passed a task function at instantiation. 
The TaskWrapper class interacts with the freeRTOS api, its public methods can be used to start, suspend, delete, and do various other things
to the task the TaskWrapper object is managing. 

## Getting Started
<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Adding to Project
1. Create a "components" directory in the root workspace directory of your esp-idf project if it does not exist already.  

   In workspace directory:     
   ```sh
   mkdir components
   ```


2. Cd into the components directory and clone the TaskWrapper repo. 

   ```sh
   cd components
   git clone https://github.com/myles-parfeniuk/task_wrapper.git
   ```
<p align="right">(<a href="#readme-top">back to top</a>)</p>

<!-- USAGE EXAMPLES -->

## Usage
<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Quick Start
This is intended to be a quick-guide, api documentation generated with doxygen can be found in the documentation directory of the master branch.  
<p align="right">(<a href="#readme-top">back to top</a>)</p>

#### Instantiating a TaskWrapper Object
To instantiate a TaskWrapper object, first declare a task function, then pass it to the TaskWrapper constructor. 

   Example syntax:  

1. Task function with void input parameter:  

    ```cpp
      void task_function();
      TaskWrapper<> my_task(&task_function);
    ```  

2. Task function with non-void input parameter:  

    ```cpp
      //where T is any data type, for ex. uint8_t, bool, MyClass, etc...
      void task_function(T *);
      T input_argument = some_value;
      TaskWrapper<T> my_task(&task_function, &input_argument);
    ```  

3. Task function that is a class method:  

    ```cpp
      //in MyClass.hpp
      Class MyClass
      {
        public:
        MyClass(); //default constructor
        private:
        TaskWrapper<MyClass> task;
        void task_function();
      };

      //in MyClass.cpp
      MyClass::MyClass():
      task(std::bind(&MyClass::task_function, this))
      {
        task.start_task(); //starts task when MyClass object is created (optional)
      }

      void MyClass::task_function()
      {
        //task code here
      } 
    ```  
<p align="right">(<a href="#readme-top">back to top</a>)</p>

#### Task Configuration
Tasks can be configured with various parameters such as stack size, priority, and the id of the core they should be executed on. 
There are two ways to accomplish this.

1. By passing a task_config_t struct as a secondary argument into a TaskWrapper constructor:  

    ```cpp
    task_config_t task_conf =
    {
      .stack_size = default_stack_size, ///<stack size of task
      .priority = default_priority;; ///<priority of the task
      .core_id = default_core_id; ///<index number of the CPU task should be pinned to
      .logging_en = false;  ///<whether or not logging is enabled, prints debug statements to terminal
      .name = "unamed"; ///<name of TaskWrapper object, used in log statements
    };
    TaskWrapper<> my_task(&task_function, task_conf); //instantiate task with above settings
    ```

    The default settings can been seen in TaskConfigDefs.hpp


2. By calling various TaskWrapper methods off an existing TaskWrapper object:  
    
    ```cpp
    my_task.set_stack_size(1024); ///set task stack depth to 1024 bytes
    my_task.set_priority(2); ///set priority of task to 2
    ```
<p align="right">(<a href="#readme-top">back to top</a>)</p>

#### Interacting with Tasks

Tasks can be stopped, started, suspended, and interacted with in various ways by calling TaskWrapper methods of an existing 
TaskWrapper object:  

  ```cpp
   my_task.start_task(); ///creates new task according to task configuration
   my_task.suspend_task(); ///suspends task until resume_task() is called
   my_task.delete_task(); ///deletes task
  ```   
Documentation for the full list of methods can be found in the documentation directory of the master branch.
<p align="right">(<a href="#readme-top">back to top</a>)</p>

### Examples
Examples are available in the TaskWrapper directory of my esp_idf_cpp_examples repo:    

[https://github.com/myles-parfeniuk/esp_idf_cpp_examples](https://github.com/myles-parfeniuk/esp_idf_cpp_examples)

My other components ButtonDriver and DataControl also utilize TaskWrapper:

[https://github.com/myles-parfeniuk/button_driver](https://github.com/myles-parfeniuk/button_driver)
[https://github.com/myles-parfeniuk/data_control](https://github.com/myles-parfeniuk/data_control)
<p align="right">(<a href="#readme-top">back to top</a>)</p>

## License

Distributed under the MIT License. See `LICENSE.md` for more information.
<p align="right">(<a href="#readme-top">back to top</a>)</p>

## Contact

Myles Parfeniuk - myles.parfenyuk@gmail.com

Project Link: [https://github.com/myles-parfeniuk/data_control](https://github.com/myles-parfeniuk/task_wrapper)
<p align="right">(<a href="#readme-top">back to top</a>)</p>