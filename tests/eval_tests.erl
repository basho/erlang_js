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

null_define_test_() ->
    [{setup, fun test_util:port_setup/0,
      fun test_util:port_teardown/1,
      [fun() ->
               P = test_util:get_thing(),
               ?assertMatch(ok, js:define(P, "")),
               erlang:unlink(P) end]}].


function_test_() ->
    [{setup, fun test_util:port_setup/0,
      fun test_util:port_teardown/1,
      [fun() ->
               P = test_util:get_thing(),
               ?assertMatch(ok, js:define(P, <<"function add_two(x, y) { return x + y; };">>)),
               ?assertMatch({ok, 95}, js:call(P, <<"add_two">>, [85, 10])),
               ?assertMatch({ok, <<"testing123">>}, js:call(P, <<"add_two">>, [<<"testing">>, <<"123">>])),
               erlang:unlink(P) end,
       fun() ->
               P = test_util:get_thing(),
               ?assertMatch(ok, js:define(P, <<"var f = function(x, y) \n{ return y - x; \n};">>)),
               ?assertMatch({ok, 75}, js:call(P, <<"f">>, [10, 85])),
               erlang:unlink(P) end,
       fun() ->
               P = test_util:get_thing(),
               ?assertMatch(ok, js:define(P, <<"function get_first(data) { return data[\"first\"]; };">>)),
               Data = {struct, [{<<"first">>, <<"abc">>}]},
               ?assertMatch({ok, <<"abc">>}, js:call(P, <<"get_first">>, [Data])),
               erlang:unlink(P) end,
      fun() ->
              %% Regression test case for embedded error properties in function return values
              P = test_util:get_thing(),
              ?assertMatch(ok, js:define(P, <<"function return_error_property() { return [{\"value\": \"some_value\", \"list\": [{\"error\": \"some_error\"}]}]; }">>)),
              ?assertMatch({ok,[{struct,[{<<"value">>,<<"some_value">>},{<<"list">>,[{struct,[{<<"error">>,<<"some_error">>}]}]}]}]}, js:call(P, <<"return_error_property">>, [])),
              erlang:unlink(P) end      
      ]      
     }].

binding_test_() ->
    [{setup, fun test_util:port_setup/0,
      fun test_util:port_teardown/1,
      [fun() ->
               P = test_util:get_thing(),
               ?assertMatch(ok, js:define(P, <<"var c = 100;function constant_mult(x) { return x * c; }">>)),
               ?assertMatch({ok, 200}, js:call(P, <<"constant_mult">>, [2])),
               ?assertMatch({ok, 1000}, js:call(P, <<"constant_mult">>, [2], [{<<"c">>, 500}])),
               erlang:unlink(P) end,
       fun() ->
               P = test_util:get_thing(),
               ?assertMatch(ok, js:define(P, <<"function constant_div(x) { return x / q; }">>, [{<<"q">>, 2}])),
               ?assertMatch({ok, 5}, js:call(P, <<"constant_div">>, [10])),
               ?assertMatch({ok, 3}, js:call(P, <<"constant_div">>, [9], [{<<"q">>, 3}])),
               erlang:unlink(P) end]}].

charset_test_() ->
    [{setup, fun test_util:port_setup/0,
      fun test_util:port_teardown/1,
      [fun() ->
           P = test_util:get_thing(),
           %% Kanji character
           Kanji = <<123,34,116,101,120,116,34,58,34,228,188,141,34,125,10>>,
           ?assertMatch(ok, js:define(P, <<"function foo(x) { return x; }">>)),
           ?assertMatch({ok, Kanji}, js:call(P, <<"foo">>, [Kanji])),
           erlang:unlink(P) end]}].

json_test_() ->
  [fun() ->
       Struct = {struct, [{<<"test">>, <<"1">>}]},
       ?assertMatch(Struct, js_mochijson2:decode(js_mochijson2:encode(Struct))) end].

ejslog_test_() ->
    [{setup, fun test_util:port_setup/0,
      fun test_util:port_teardown/1,
      [fun() ->
               P = test_util:get_thing(),
               [] = os:cmd("rm -f /tmp/eval_tests.log"),
               ?assertEqual({ok, true},
                            js_driver:eval_js(P, <<"ejsLog('/tmp/eval_tests.log', 'Hello')">>)),
               ?assert(filelib:is_file("/tmp/eval_tests.log")),
               erlang:unlink(P)
       end]}].


error_test_() ->
    [{setup, fun test_util:port_setup/0,
      fun test_util:port_teardown/1,
      [fun() ->
               P = test_util:get_thing(),
               {error, ErrorDesc} = js:define(P, <<"functoin foo(x, y) { return true; };">>),
               ?assert(verify_error(ErrorDesc)),
               erlang:unlink(P) end,
       fun() ->
               P = test_util:get_thing(),
               ?assertMatch(ok, js:define(P, <<"function foo(x, y) { return true; };">>)),
               {error, ErrorDesc} = js:eval(P, <<"foo(100, 200,);">>),
               ?assert(verify_error(ErrorDesc)),
               erlang:unlink(P) end,
       fun() ->
               P = test_util:get_thing(),
               {error, ErrorDesc} = js:define(P, <<"functoin foo() { return \"oops\"; };">>),
               ?assert(verify_error(ErrorDesc)),
               erlang:unlink(P) end,
       fun() ->
               P = test_util:get_thing(),
               js:define(P, <<"function foo() { return [{\"error\":\"notfound\"}]; }">>),
               ?assertMatch({error, <<"[{\"error\":\"notfound\"}]">>}, js:call(P, <<"foo">>, [])),
               erlang:unlink(P) end,
       fun() ->
               P = test_util:get_thing(),
               {error, ErrorDesc} = js:eval(P, <<"blah(\"wubba\");">>),
               ?assert(verify_error(ErrorDesc)),
               erlang:unlink(P) end]}].


%% Internal functions
verify_error([{<<"lineno">>, LineNo},
              {<<"message">>, Msg},
              {<<"source">>, Source}]) when is_number(LineNo),
                                                          is_binary(Msg),
                                                          is_binary(Source) ->
    true;
verify_error(Error) ->
    ?debugFmt("Error: ~p~n", [Error]),
    false.
