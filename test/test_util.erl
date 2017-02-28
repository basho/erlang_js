-module(test_util).

-export([port_setup/0, port_setup/1, port_teardown/1, null_teardown/1, get_thing/0]).

port_setup() ->
    erlang_js:start(),
    port_setup(8).

port_setup(Size) ->
  {ok, P} = js_driver:new(8, Size),
  start_thing_holder(P),
  P.

port_teardown(P) ->
  thing_holder ! stop,
  erlang:port_connect(P, self()),
  js_driver:destroy(P).

null_teardown(_) ->
  thing_holder ! stop,
  ok.

get_thing() ->
  thing_holder ! {get_thing, self()},
  receive
    Thing ->
      if
        is_port(Thing) ->
          erlang:port_connect(Thing, self());
        true ->
          ok
      end,
      Thing
  end.

%% Internal functions
start_thing_holder(Thing) ->
  if
    is_port(Thing) ->
      erlang:unlink(Thing);
    true ->
      ok
  end,
  Pid = spawn(fun() -> thing_holder(Thing) end),
  register(thing_holder, Pid),
  Pid.

thing_holder(Thing) ->
  receive
    {get_thing, Caller} ->
      Caller ! Thing,
      thing_holder(Thing);
    stop ->
      ok
  end.
