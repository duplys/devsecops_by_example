# Cppcheck

[Cppcheck](https://cppcheck.sourceforge.io/) is an open source static analysis tool for C and C++. [Static code analysis](https://en.wikipedia.org/wiki/Static_program_analysis) is an automated, tool-based analysis of computer programs performed without executing them.

As a unique feature, Cppcheck uses [unsound flow-sensitive analysis](https://cppcheck.sourceforge.io/#unique). This allows Cppcheck to find bugs that other tools, which use path-sensitive analysis, might miss. Path-sensitive analysis collects path information indicating the feasibility or infeasibility of a path, and only reports findings from feasible paths to reduce the number of false positives. As an example, for the following code sample, most tools can determine that the array index will be 1000 and, as a result, will lead to a buffer overflow:

```C
void foo(int x)
{
    int buf[10];
    if (x == 1000)
        buf[x] = 0; // <- ERROR
}
```

However, Cppcheck will also diagnose this:

```C
void foo(int x)
{
    int buf[10];
    buf[x] = 0; // <- ERROR
    if (x == 1000) {}
}
```

Cppcheck detects undefined behavior (e.g., dead pointers, division by zero, integer overflows, null pointer dereferences, out of bounds, uninitialized variables) and security vulnerabilities (e.g., buffer errors, improper access control, information leaks, improper input validation). Moreover, Cppcheck can check for compliance with coding standards standards Misra C 2012 and Misra C++ 2008.

You can download Cppcheck from [SourceForge](https://sourceforge.net/projects/cppcheck/) or [GitHub](https://github.com/danmar/cppcheck). You can also install Cppcheck [using `apt`, `yum` or `brew`](https://cppcheck.sourceforge.io/).


## Installation
Assuming you're on a Debian or Ubuntu operating system, you can install Cppcheck like this:

```bash
$ sudo apt-get install cppcheck -y
```

## Vulnerable application example


```C
#include <stdio.h>

void foo(int x)
{
    int buf[10];
    if (x == 1000)
        buf[x] = 0; // <- ERROR
}

void bar(int x)
{
    int buf[10];
    buf[x] = 0; // <- ERROR
    if (x == 1000) {}
}

int main() {
    char a[10];
    a[10] = 0;

    printf("Hello world!\n");
    
    return 0;
}
``` 

## Running Cppcheck
You can now run Cppcheck on the vulnerable application in the `vuln-app` directory:
```bash
$ cppcheck vuln-app
```

## Example terminal output
If your setup is correct, you should see this:
```bash
$ cppcheck vuln-app
Checking vuln-app/file1.c...
[vuln-app/file1.c:5]: (error) Array 'a[10]' accessed at index 10, which is out of bounds.
```

## XML report
Cppcheck can also output its findings as XML:
```bash
$ cppcheck --xml vuln-app
<?xml version="1.0" encoding="UTF-8"?>
<results>
Checking vuln-app/file1.c...
    <error file="vuln-app/file1.c" line="5" id="arrayIndexOutOfBounds" severity="error" msg="Array &apos;a[10]&apos; accessed at index 10, which is out of bounds."/>
</results>
```
