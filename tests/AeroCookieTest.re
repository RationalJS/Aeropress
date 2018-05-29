open BsOspec.Cjs;
open AeroRoutes;
open AeroRoutes.Router;
open AeroRoutes.Middleware;

open TestHelper;

describe("AeroCookie", () => {

  test("middleware", () => {
    let router = route
      &&& get("/hello")
      &&& AeroCookie.m
      &&& (r => r |.
        sendJson'(200, { "c": r.ctx##cookies |. Js.Dict.unsafeGet("my-cookie") })
      );

    makeReq(
      ~headers=Js.Dict.fromList([("cookie", "my-cookie=yum!")]),
      "GET", "/hello")
    |. router
    |. getRes( expectJson({ "c": "yum!" }) );
  });

});
