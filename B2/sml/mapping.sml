fun findValue s nil = raise NotDefined
    | findValue s (h::t) =
    if h = s then value
        else findValue s t