##Example Usage 

```reason
open History;

let history = createBrowserHistory ();
let unsub = subscribe history (fun action location => {
  Js.log (actionToString action);
  Js.log location.key; 
  Js.log (Browser.History.getState Browser.history);
});

/* blocker function can return either | Prompt message | Block | Skip  */
let unblock = block history (fun action location => {
  Prompt ("You sure you want to" ^ actionToString(action) ^ "to " ^ createHref(location) ^ "?");
}); 

/* This will show a prompt before transitioning */
push history "/route1?query=yolo#someid" state::{"first": "state"}; 

unblock (); 

push history "/route2#someid" state::{"second": "state"}; 
```