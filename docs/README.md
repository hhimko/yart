# Yet Another Ray Tracer 
YART thesis LaTeX project.

## Building the document
The only requirement for building the document is a LaTeX distribution in your system's PATH. It is strongly recommended to use [TeX Live], which is a free and comprehensive distribution for all major platforms that comes packaged with [Perl] for convenience. When using a different distribution such as [MiKTeX], you will need to install Perl yourself, which is required for running the `latexmk` script.

### Building from shell
To build the document from the terminal, simply run the following command:

```shell
> latexmk main.tex -pdf -outdir=build 
```

### Building with LaTeX Workshop
When working from Visual Studio Code, it is recommended to use the [LaTeX Workshop] extension by James Yu for building. To install LaTeX Workshop for your OS, follow the [installation guide]. 

To not clutter the project directory, it is recommended to set the `latex-workshop.latex.outDir` setting to `"%DIR%/build"`. You can modify VSC settings through the `Preferences > Settings` menu, or alternatively by directly editing the global `settings.json` file.

## License
YART thesis is licensed under the #TODO license.

Author: Dawid Cyganek [@hhimko]




[TeX Live]: https://www.tug.org/texlive/
[Perl]: https://www.perl.org/
[MiKTeX]: https://miktex.org/
[LaTeX Workshop]: https://marketplace.visualstudio.com/items?itemName=James-Yu.latex-workshop
[installation guide]: https://github.com/James-Yu/LaTeX-Workshop/wiki/Install
[@hhimko]: https://github.com/hhimko
