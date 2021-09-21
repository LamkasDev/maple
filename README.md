# Maple
[![CI](https://img.shields.io/github/workflow/status/LamkasDev/maple/run-tests)](https://github.com/LamkasDev/maple)
[![Release](https://img.shields.io/github/v/release/LamkasDev/maple)](https://github.com/LamkasDev/maple/releases)
[![Contributors](https://img.shields.io/github/contributors/LamkasDev/maple)](https://github.com/LamkasDev/maple)
[![Issues](https://img.shields.io/bitbucket/issues-raw/LamkasDev/maple)](https://github.com/LamkasDev/maple/issues)

## Description
Maple is an interpreted programming language written in C++, supporting variety of basic features with Javascript-like syntax.

## Building
Building is done via Make with <code>make</code> or <code>make debug</code> (includes debugging symbols).

**Requirements:**  
<code>g++</code> 5.1 or later  
<code>make</code> 3.81 or later   

## Setting up needed tools
**Windows (64-bit):**  
Install <a href="http://mingw-w64.org/doku.php">mingw-w64</a> (64-bit version only)  
Install <a href="https://chocolatey.org/install">Chocolatey</a> and then install with <code>choco install make</code>  
Also download <a href="https://chocolatey.org/install">maple_devtools-win.zip</a> and extract the files into <code>C:/Program Files/maple_devtools-win</code>  

**Linux (Ubuntu):**  
Run <code>sudo apt-get install g++ build-essential libssl-dev zlib1g-dev</code>

## CLI Commands
| Command                                 | Description                                  |
| --------------------------------------- | -------------------------------------------- |
| `maple-shell`                           | Stars the standart shell.                    |
| `maple-shell -d`                        | Stars the standart shell with debug info.    |
| `maple-shell -v`                        | Shows the currently installed version.       |
| `maple-shell -tests`                    | Runs the default test suite.                 |
| `maple-shell -run [file_name]`          | Runs the specified file.                     |
            
## Platforms
<table>
    <tr>
        <th>Windows</th>
        <th>Linux</th>
        <th>macOS</th>
    </tr>
    <tr>
        <th>✅</th>
        <th>✅</th>
        <th>❌</th>
    </tr>
</table>

* macOS support is not planned, unless somebody adds support for it.
            
## Contributing
If you want a feature added or you found a bug, make a new <a href="https://github.com/LamkasDev/maple/issues">Issue</a>.  
If you want to contribute, make a new <a href="https://github.com/LamkasDev/maple/pulls">Pull Request</a>.  
There are no guidelines or any of the sort and contributing is highly encougaraged!

## License
Maple is licensed under the [GNU General Public License v3.0](https://github.com/LamkasDev/maple/blob/master/LICENSE).
