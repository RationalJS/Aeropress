open BsOspec.Cjs;
open AeroRoutes;
open AeroRoutes.Router;
open AeroRoutes.Middleware;

open TestHelper;

describe("Basic route matching", () => {

  test("single route", () => {
    let router = route &&& get("/hello") &&& literal("hi");

    makeReq("GET", "/hello")
    |. router
    |. getRes( expectJson("hi") );
  });

  test("multiple routes", () => {
    let router = route
      &&& get("/one") &&& literal(10)
      ||| get("/two") &&& literal(20);

    makeReq("GET", "/one")
    |. router
    |. getRes( expectJson(10) );

    makeReq("GET", "/two")
    |. router
    |. getRes( expectJson(20) );
  });

  testAsync("async route", done_ => {
    let slowLiteral = (x) => (r) =>
      async @@ delay(50, () => r |> literal(x));

    let router = route &&& get("/hello") &&& slowLiteral("hi");

    makeReq("GET", "/hello")
    |. router
    |. getRes( expectJson("hi") >>% done_ );
  });

  testAsync("double async route", done_ => {
    let pause = (r) => {
      delay(50, () => next(r)) |. async;
    };
    let slowLiteral = (x) => (r) =>
      delay(50, () => r |> literal(x)) |. async;

    let router = route &&& get("/goodbye") &&& pause &&& slowLiteral("bye");

    makeReq("GET", "/goodbye")
    |. router
    |. getRes( expectJson("bye") >>% done_ );
  });

  test("middleware adding to context", () => {
    let yield_x = (r) => next_assign(r, { "x": 10 });

    let router = route
      &&& get("/")
      &&& yield_x
      &&& (r => r |. sendJson'(200,r.ctx##x));

    makeReq("GET", "/")
    |. router
    |. getRes( expectJson(10) );
  });

});

describe("Route primitive composition", () => {

  test("get & prefix (1)", () => {
    let router = route
    &&& prefix("/one")
    &&& get("/two")
    &&& literal("gp_1")
    ||| route &&& literal("not found");

    makeReq("GET", "/one/two")
    |. router
    |. getRes( expectJson("gp_1") );

    makeReq("GET", "/one")
    |. router
    |. getRes( expectJson("not found") );
  });

  test("get & prefix (2)", () => {
    let router = route
    &&& prefix("/one")
    &&& prefix("/two")
    &&& get("/three")
    &&& literal("gp_2")
    ||| route &&& literal("not found");

    makeReq("GET", "/one/two/three")
    |. router
    |. getRes( expectJson("gp_2") );

    makeReq("GET", "/one/two")
    |. router
    |. getRes( expectJson("not found") );
  });

  test("get, prefix, & param", () => {
    let router = route
    &&& prefix("/users/")
    &&& param(paramValue => { "myId": int_of_string(paramValue) })
    &&& get("/test-route")
    &&& (r => r |. sendJson'(200, { "id": r.ctx##myId }))
    ||| route &&& literal("not found");

    makeReq("GET", "/users/123/test-route")
    |. router
    |. getRes( expectJson({ "id": 123 }) );

    makeReq("GET", "/users/test-route")
    |. router
    |. getRes( expectJson("not found") );
  });

  test("get, prefix, & alter", () => {
    let router = route
    &&& prefix("/about/")
    &&& (get("me") ||| get("you"))
    &&& alterUrl(url => url ++ ".html")
    &&& (r => r |. sendJson'(200, { "page": r.req.url }))
    ||| route &&& literal("not found");

    makeReq("GET", "/about/me")
    |. router
    |. getRes( expectJson({ "page": "/about/me.html" }) );

    makeReq("GET", "/about/you")
    |. router
    |. getRes( expectJson({ "page": "/about/you.html" }) );
  })
});
