[Return to README.md](README.md)

# Create my application

Set up Vim shortcuts to make it as easy as possible to rebuild
and run:

- `;<Space>` to build:
    - call `make`
    - open quickfix if there are errors:
        - use Vim `getqflist` to get a list of Vim dictionaries
          for each line of build output
        - for each dictionary, pull out the value pair for key
          `valid` into a list of all "valids"
        - look at the join of that list
            - if any value is not zero, the join is not zero,
              indicating an error
- `;r<Space>` to run the build
    - calls `build/main`

