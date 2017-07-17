type location = {
  pathname: string, 
  hash: string, 
  search: string
};

let parsePath = fun 
| ""  => {pathname: "/", hash: "", search: ""}
| path => {
  let (hash, pathname) = 
    if (String.contains path '#') {
      let hashIndex = String.index path '#';
      let hashLength = (String.length path) - hashIndex;
      let hash = String.sub path hashIndex hashLength;
      let pathname = String.sub path 0 hashIndex;
      (hash, pathname)
    } else {
      ("", path)
    };

  let (search, pathname) = 
    if (String.contains pathname '?') {
      let searchIndex = String.index pathname '?';
      let searchLength = (String.length pathname) - searchIndex;
      let search = String.sub pathname searchIndex searchLength;
      let pathname = String.sub pathname 0 searchIndex;
      (search, pathname)
    } else {
      ("", pathname)
    };
    
  {
    pathname: pathname, 
    search: search,
    hash: hash,
  };
};
