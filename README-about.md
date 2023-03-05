[Return to README.md](README.md)

# About

I make this because I need a drawing application.
This is a "whatever gets the job done quickest" sort of thing.

I think it will end up being faster to write something myself
than to research the available free diagram-drawing options and
learn how to use one.

I use the Dear ImGui example code as my main entry point and let
my "drawing application" be like a little library pulled in by
the Dear ImGui example code.

I will use the Dear ImGui SDL example because I'm familiar with
SDL. In fact, keeping things super-simple for me, I'm also using
the SDL renderer, no GPU lib.

But in theory I should be able to swap this out with any of the
examples and then be able to build for a platform that uses that
particular backend. Like the `emscripten` example to compile to
WebAssembly and turn this into a Web application (though I have
no reason to do that or any idea how to make web applications).


