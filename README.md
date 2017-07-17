# TODO

- [x] push 
- [x] replace 
- [x] createBrowserHistory 
- [x] listen/unlisten
- [x] block/unblock
- [x] createHref
- [ ] createHashHistory

# Example Usage 

```reason
open History;

let history = createBrowserHistory ();

let unsub = subscribe history (fun action location => {
  Js.log (actionToString action);
  Js.log location.key; 
  Js.log (Browser.History.getState Browser.history);
});

let unblock = block history (fun action location => {
  /* Return `Prompt message` variant to show a `window.confirm` prompt */
  Prompt ("You sure you want to" ^ actionToString(action) ^ "to " ^ createHref(location) ^ "?");
  /* or just block this transition */
  Block
  /* or let it pass */
  Pass
}); 

/* This will show a prompt before transitioning */
push history "/route1?query=yolo#someid"; 

unblock (); 

/* Can also pass in a state JS object*/
push history "/route2#someid" state::{"second": "state"}; 
```