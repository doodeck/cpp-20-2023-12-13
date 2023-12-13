# Szkolenie - C++20 #

## Docs
* https://infotraining.github.io/docs-cpp-20/intro.html
* https://infotraining.bitbucket.io/cpp-20/

## Video

* https://infotrainingszkolenia-my.sharepoint.com/:v:/g/personal/krystian_piekos_infotraining_pl/EWudr8lxr6xGhUlawqdqpsUBW1cBrRBmRTC9E8rmYfVKJg?e=nt0He3&nav=eyJyZWZlcnJhbEluZm8iOnsicmVmZXJyYWxBcHAiOiJTdHJlYW1XZWJBcHAiLCJyZWZlcnJhbFZpZXciOiJTaGFyZURpYWxvZy1MaW5rIiwicmVmZXJyYWxBcHBQbGF0Zm9ybSI6IldlYiIsInJlZmVycmFsTW9kZSI6InZpZXcifX0%3D

* https://infotrainingszkolenia-my.sharepoint.com/:v:/g/personal/krystian_piekos_infotraining_pl/ERJFm1CkvdhFktQnyzswbakBWQl_LysEUGmIneuSTzC1pw?e=Vly1f2&nav=eyJyZWZlcnJhbEluZm8iOnsicmVmZXJyYWxBcHAiOiJTdHJlYW1XZWJBcHAiLCJyZWZlcnJhbFZpZXciOiJTaGFyZURpYWxvZy1MaW5rIiwicmVmZXJyYWxBcHBQbGF0Zm9ybSI6IldlYiIsInJlZmVycmFsTW9kZSI6InZpZXcifX0%3D

* https://infotrainingszkolenia-my.sharepoint.com/:v:/g/personal/krystian_piekos_infotraining_pl/EZMWz-7BF-9Mou4UJUZk83UBfXNTw83VmhLrPuAdys7rqw?e=po9u9f&nav=eyJyZWZlcnJhbEluZm8iOnsicmVmZXJyYWxBcHAiOiJTdHJlYW1XZWJBcHAiLCJyZWZlcnJhbFZpZXciOiJTaGFyZURpYWxvZy1MaW5rIiwicmVmZXJyYWxBcHBQbGF0Zm9ybSI6IldlYiIsInJlZmVycmFsTW9kZSI6InZpZXcifX0%3D

## Konfiguracja środowiska

Proszę wybrać jedną z poniższych opcji:

### Lokalna

Przed szkoleniem należy zainstalować:

#### Kompilator C++ wspierający C++20 - do wyboru:
  * Visual Studio 2022
    * przy instalacji należy zaznaczyć opcje:
      * Desktop development with C++
      * C++ CMake tools for Windows
      * vcpkg package manager
      * C++ Modules

  * gcc-13
    * [CMake > 3.25](https://cmake.org/)
    * proszę sprawdzić wersję w linii poleceń        
  
      ```
      cmake --version
      ```
      
    * IDE: Visual Studio Code
      * [Visual Studio Code](https://code.visualstudio.com/)
      * Zainstalować wtyczki
        * C/C++ Extension Pack
        * Live Share

### Docker + Visual Studio Code

Jeśli uczestnicy szkolenia korzystają w pracy z Docker'a, to należy zainstalować:

#### Docker
* https://www.docker.com/get-started/

#### Visual Studio Code

* [Visual Studio Code](https://code.visualstudio.com/)
* Zainstalować wtyczki
  * Live Share
  * Dev Containers ([wymagania](https://code.visualstudio.com/docs/devcontainers/containers#_system-requirements))
    * po instalacji wtyczki - należy otworzyć w VS Code folder zawierający sklonowane repozytorium i
      z palety poleceń (Ctrl+Shift+P) wybrać opcję **Dev Containers: Rebuild and Reopen in Container**
