(progn
  (defun loop (s e)
    (cond ((= s e) 0)
          ((> s e) 0)
          (t (progn
               (print s)
               (loop (+ s 1) e)
               )
             )
          )
    )
  

  (loop 3 10)
)