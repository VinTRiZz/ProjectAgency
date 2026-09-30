# ProjectAgency
Project, designed to develop programming projects (at first vision -- C projects, for time optimisation).  
The system uses Ollama to work, may use many subservers (for this purposes, install on a target device neural net using Ollama that will actually process requests and AIBackend application from this repository to handle it as a part of a system; later you will be able to add it in ControlWindow).

## Contents
1. ControlWindow: GUI thin client interaction with AIManager and utility functions (such as AI config constructor)
    1. AIConfigGenerator: Allows to create, generate and manipulate with configuration files (*.mf), during AIBackend configuring. Works with AIManager and handles requests like "Create config for C++ engeneer, based on DeepSeek-R1", "What configs we have?".
    2. AIStatusManager: Watching generation state in real time
    3. AIBusinessManager: Manage roles of AI in system, configs of them, AI type (DeepSeek, LLama, etc.), network configuration
    4. TaskManager: Manages all tasks (in process, history, result storing)

2. AIManager: Management backend for status, creating tasks, etc. (controls and asks whole system). Works with AIOrchestrator directly
    1. AIOrchestrator: Works with AIBackend instances, creating network based on them. Handles requests like "What AI types we have?" and "How many workers are free now?", etc.
    3. ProjectMaster: Module for project analyse and creating files/directories, used to search for functions, classes, libraries, etc. (AI use it to find, what classes, libraries, functions it can use while developing).

3. AIBackend: Simple WS client, that connects Ollama-created AI on a client with ecosystem. Works with AI stated on current device. Just handles requests like "Reconfigure as C++ engeneer." or "Create SQLite backend class".

### READY
1. AIBackend, requesting Ollama by http
2. AIManager connection to AIBackend
3. ControlWindow, able to get AIManager configuration and simple show AIBackend instances

### PLANS
1. Status control of AIBackned instances
2. Requesting to a input model from ControlWindow
3. Filesystem and command execution managing
4. Orchestrating functionality in AIManager
5. Context manipulations (such as caching info about project)
