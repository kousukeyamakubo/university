fun compute s mapL =
    let
	fun EXP nil = raise SyntaxError
          | EXP (h::h1::t) =
            if isInt h then (toInt h, [h1]@t)
            else if h = "(" then 
                if h1 = "+" orelse h1 = "-" orelse h1 = "*" orelse h1 = "/" then COMP (h::[h1]@t)
                else if isAlp h1 then FUNC (h::[h1]@t)
                else raise SyntaxError
            else if h = "+" orelse h = "-" orelse h = "*" orelse h = "/" then COMP (h::[h1]@t)
            else if h = "fact" orelse h = "fibo" then FUNC (h::[h1]@t)
            else if isAlp h then findValue h mapL 
            else raise SyntaxError

	and COMP nil = raise SyntaxError
          | COMP (h::t) =
            if h = "(" then
                let
                    val (v1, t1) = EXP t

                    fun splitList lst =
                        case lst of
                            [] => raise SyntaxError
                        | hd::tl => (hd, tl)

                    val (head,t2) = splitList t1
                in
                    if head = ")" then
                        let
                        in
                            (v1, t2)
                        end
                    else
                        raise SyntaxError
                end

            else if h = "+" then
                let
                    val (v1,t1) = EXP t
                    val (v2,t2) = EXP t1
                in
                    (v1 + v2, t2)
                end

            else if h = "-" then
                let
                    val (v1,t1) = EXP t
                    val (v2,t2) = EXP t1
                in
                    (v1 - v2, t2)
                end 

            else if h = "*" then
                let
                    val (v1,t1) = EXP t
                    val (v2,t2) = EXP t1
                in
                    (v1 * v2, t2)
                end 

            else if h = "/" then
                let
                    val (v1,t1) = EXP t
                    val (v2,t2) = EXP t1
                in
                    (v1 div v2, t2)
                end 

            else raise SyntaxError

    and FUNC nil = raise SyntaxError
          | FUNC (h::t) =
            if h = "(" then
                let
                    val (v1,t1) = EXP t

                    fun splitList lst =
                        case lst of
                            [] => raise SyntaxError
                        | hd::tl => (hd, tl)

                    val (head,t2) = splitList t1
                in
                    if head = ")" then
                        let
                        in
                            (v1, t2)
                        end
                    else
                        raise SyntaxError
                end
            
            else if h = "fact" then
                let
                    val (v1,t1) = EXP t
                    val v2 = fact v1
                in
                    (v2, t1)
                end

            else if h = "fibo" then
                let
                    val (v1,t1) = EXP t
                    val v2 = fibo v1
                in
                    (v2, t1)
                end
            else raise SyntaxError

    in
	    let
            val (result,rest) = EXP (separate s)
	    in
            if rest = nil then result else raise SyntaxError
	    end
    end;