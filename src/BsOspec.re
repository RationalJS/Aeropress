
module Common = {
  [@bs.module "ospec"]
  external describe : (string, (unit => unit)) => unit = "spec";

  [@bs.module "ospec"]
  external testOnly : (string, (unit => unit)) => unit = "only";
  [@bs.module "ospec"]
  external testAsyncOnly : (string, (unit => unit) => 'a) => unit = "only";
  [@bs.module "ospec"]
  external testAsyncLongOnly : (string, (unit => unit, int => unit) => 'a) => unit = "only";

  [@bs.module "ospec"]
  external before : (unit => 'a) => unit = "";
  [@bs.module "ospec"]
  external after : (unit => 'a) => unit = "";

  [@bs.module "ospec"]
  external beforeAsync : (('a => unit) => 'b) => unit = "before";
  [@bs.module "ospec"]
  external afterAsync : (('a => unit) => 'b) => unit = "after";


  [@bs.module "ospec"]
  external beforeEach : (unit => 'a) => unit = "";
  [@bs.module "ospec"]
  external afterEach : (unit => 'a) => unit = "";

  [@bs.module "ospec"]
  external beforeEachAsync : (('a => unit) => 'b) => unit = "beforeEach";
  [@bs.module "ospec"]
  external afterEachAsync : (('a => unit) => 'b) => unit = "afterEach";
};

type desc_fn = [@bs] (string) => unit;
type checker('a) = {
  .
  "equals": [@bs.meth] ('a) => desc_fn,
  "deepEquals": [@bs.meth] ('a) => desc_fn,
  "notEquals": [@bs.meth] ('a) => desc_fn,
  "notDeepEquals": [@bs.meth] ('a) => desc_fn,
};

module type Checker = {
  let make : ('a) => checker('b);
};

module Assertions = (Checker : Checker) => {
  let equals = (expected, ~m=?, actual) => switch(m) {
    | Some(message) =>
      let desc = Checker.make(actual)##equals(expected);
      desc(. message)
    | None => Checker.make(actual)##equals(expected) |> ignore
  };

  let deepEquals = (expected, ~m=?, actual) => switch(m) {
    | Some(message) =>
      let desc = Checker.make(actual)##deepEquals(expected);
      desc(. message)
    | None => Checker.make(actual)##deepEquals(expected) |> ignore
  };

  let notEquals = (expected, ~m=?, actual) => switch(m) {
    | Some(message) =>
      let desc = Checker.make(actual)##notEquals(expected);
      desc(. message)
    | None => Checker.make(actual)##notEquals(expected) |> ignore
  };

  let notDeepEquals = (expected, ~m=?, actual) => switch(m) {
    | Some(message) =>
      let desc = Checker.make(actual)##notDeepEquals(expected);
      desc(. message)
    | None => Checker.make(actual)##notDeepEquals(expected) |> ignore
  };
};

module Cjs = {
  include Common;
  include Assertions({
    [@bs.module]
    external make : ('a) => checker('b) = "ospec";
  });
  [@bs.module]
  external test : (string, (unit => 'a)) => unit = "ospec";
  [@bs.module]
  external testAsync : (string, (unit => unit) => 'a) => unit = "ospec";
  [@bs.module]
  external testAsyncLong : (string, (unit => unit, int => unit) => 'a) => unit = "ospec";
};

module Esm = {
  include Common;
  include Assertions({
    [@bs.module "ospec"]
    external make : ('a) => checker('b) = "default";
  });

  [@bs.module "ospec"]
  external test : (string, (unit => 'a)) => unit = "default";
  [@bs.module "ospec"]
  external testAsync : (string, (unit => unit) => 'a) => unit = "default";
  [@bs.module "ospec"]
  external testAsyncLong : (string, (unit => unit, int => unit) => 'a) => unit = "default";
};
