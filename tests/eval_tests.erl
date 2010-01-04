-module(eval_tests).

-include_lib("eunit/include/eunit.hrl").

var_test_() ->
  [{setup, fun test_util:port_setup/0,
    fun test_util:port_teardown/1,
    [fun() ->
         P = test_util:get_thing(),
         ?assertMatch(ok, js:define(P, <<"var x = 100;">>)),
         ?assertMatch({ok, 100}, js:eval(P, <<"x;">>)),
         erlang:unlink(P) end]}].

function_test_() ->
  [{setup, fun test_util:port_setup/0,
    fun test_util:port_teardown/1,
    [fun() ->
         P = test_util:get_thing(),
         ?assertMatch(ok, js:define(P, <<"function add_two(x, y) { return x + y; };">>)),
         ?assertMatch({ok, 95}, js:call(P, <<"add_two">>, [85, 10])),
         ?assertMatch({ok, <<"testing123">>}, js:call(P, <<"add_two">>, [<<"testing">>, <<"123">>])),
         erlang:unlink(P) end]}].
