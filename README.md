# Why port to Reason instead of binding?

✔ Idiomatic Reason usage throughout

✔ Type checking for all function arguments/returns/options, and location descriptors.

✔ Take advantage of Variant types for action types (POP|PUSH|REPLACE) and
blocker functions (Prompt message | Block| Pass);

# TODO

- [x] push
- [x] replace
- [x] createBrowserHistory
- [x] listen/unlisten
- [x] block/unblock (for push/replace)
- [x] createHref
- [x] block for POPs
- [ ] basename
- [ ] createHashHistory

# Example Usage

```reason

/* Example usages  */

/* state could be anything but you must define its type before calling createBrowserHistory */
type state = {mystate: string};

/* location.state is a variant of | Some state and | None  */
let logState = fun
| Some {mystate: value} => Js.log "Current state value: " ^ value
| None => Js.log "No state";

let actionToString = fun
| Pop => "POP"
| Push => "PUSH"
| Replace => "REPLACE";

let history = History.createBrowserHistory ();

let unsub = History.subscribe history (fun action location => {
  Js.log (actionToString action);
  Js.log location.key;
  logState location.state
});

push history "/route1#someid";

let unblock = History.block history (fun _action _location => {
  /* Callback function can return either */
  Prompt "Are you sure you want to leave this page??";
  /* or */
  Block
  /* or */
  Pass
});

/* This will show a prompt before transitioning */
push history "/route2" state::{mystate: "state1"};

/* Detach listeners */
unblock ();
unsub ();
```