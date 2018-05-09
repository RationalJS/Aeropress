# bs-ospec

BuckleScript bindings for the excellent and minimal [ospec testing library](https://www.npmjs.com/package/ospec). Perfect for simple testing and constructing your own, custom assertions.

## Installation

```
$ npm install --save-dev ospec bs-ospec
```

Then add `"bs-ospec"` to your `bsconfig.json` dev dependencies:

```
{
  ...
  "bs-dev-dependencies": [
    "bs-ospec"
  ]
}
```

## Example Usage

First make sure your test files names are easily distinguishable. Here are some examples:

| Format                                | Command to Run            |
| ----                                  | ----                      |
| Within a `tests/` folder              | `ospec`                   |
| Within a custom folder like `spec/`   | `ospec 'spec/**/*.bs.js'` |
| Named `MyModuleTest.re` in any folder | `ospec '**/*Test.bs.js'`  |


Next, write your tests. Ospec uses a single function `o()` to do pretty much everything. However, OCaml doesn't support overloaded functions, so bs-ospec separates each use case into its own function (it all compiles to a single function in the end).

```js
open BsOspec.Cjs;

describe("Example", () => {

  test("sync example", () => {
    f(x,y) |> equals("a correct value");
    f(x,y) |> equals("a correct value", ~m="A descriptive failure message");
    g(x,y) |> deepEquals(["another", "correct", "value"]);
  });

  testAsync("async example", done_ => {
    /* Note how we use testAsync() instead of test() !! */
    someTask()
    |> Task.run(result => {
      result |> equals("expected");
      done_()
    })
  });

  testAsyncLong("longer async example", (done_, timeout) => {
    timeout(2000) /* ospec default is 50 milliseconds. */

    someLongTask()
    |> Task.run(result => {
      result |> equals("expected");
      done_()
    })
  });
});
```

Lastly, run your test suite by running an ospec command like the table shown above.

## ES Modules

`BsOspec` supports both CommonJS and ES Modules (ESM). BuckleScript is configured to use CommonJS by default; if you are using ESM, first configure your bsconfig.json to use `es6-global`:

```json
{
  ...
  "package-specs": {
    "module": "es6-global",
    "in-source": true
  }
}
```

Then just write `open BsOspec.Esm;` instead of `open BsOspec.Cjs;` in your test files.

If you're interested in using ESM today, you can install the [esm package](https://www.npmjs.com/package/esm) and add `--require esm` to the end of your ospec command. For example:

```bash
ospec '**/*Test.bs.js' --require esm
```

## Bindings

See [the source](./src/BsOspec.re) for the full details.

Test Definitions:

- `describe` - Group a collection of tests. Not required.
- `test` - Define a **synchronous** test
- `testAsync` - Define an async test
- `testAsyncLong` - Define an async test expected to last longer than 50ms.
- `testOnly`, `testAsyncOnly`, `testAsyncLongOnly` - Define and **only** run this test. Useful for focusing on a single test.

Hooks:

- `beforeEach`, `beforeEachAsync` - Run code before each test
- `afterEach`, `afterEachAsync` - Run code after each test
- `before`, `beforeAsync` - Run code **once** before **all** tests
- `after`, `afterAsync` - Run code once after all tests

Assertions:

- `equals(expected, ~m=?, actual)` - Expect a value to equal another value. Optionally pass in `~m="my msg"` to show a custom message if the assertion fails.
- `deepEquals(expected, ~m=?, actual)` - Expect a value to **deep equal** another value.
- `notEquals(expected, ~m=?, actual)`
- `notDeepEquals(expected, ~m=?, actual)`

## Build
```
npm run build
```

## Build + Watch

```
npm run start
```


## Editor
If you use `vscode`, Press `Windows + Shift + B` it will build automatically
