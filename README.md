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

First, make sure your tests are in a folder EXACTLY named `tests/` (this limitation [should soon be lifted](https://github.com/MithrilJS/mithril.js/pull/2137)).

Next, write your tests. Ospec uses a single function `o()` to do pretty much everything. However, OCaml doesn't support overloaded functions, so bs-ospec separates each use case into its own function (it all compiles to a single function in the end).

```js
open BsOspec;

describe("Example", () => {

  test("sync example", () => {
    f(x,y) |> equals("a correct value");
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

Lastly, run `ospec` in your terminal to run your test suite.

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

- `equals(expected, actual)` - Expect a value to equal another value.
- `deepEquals(expected, actual)` - Expect a value to **deep equal** another value.
- `notEquals(expected, actual)`
- `notDeepEquals(expected, actual)`

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
