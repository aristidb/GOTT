; Copyright (C) 2005 by Rüdiger Sonderfeld <kingruedi@c-plusplus.de>

; HochBau

; base (
(use-modules (oop goops))
(use-modules (oop goops describe))

(define-class item ())
(define-class build-item (item)
  (dependencies #:init-value '()))
(define-class build-file-item (build-item)
  (source-filename #:init-value "")
  (target-filename #:init-value ""))

(define-class module ()
  (items #:init-value '()))

(define-generic build)
(define-method (build obj env))

(define-generic has-changed?)
(define-method (has-changed? obj env) #t)

(define-generic scan-dependencies)
(define-method (scan-dependencies obj env) '())

(define env nil)

(define build-item-list
  (lambda (items env)
    "builds a list of items"
    (if (not (null? items))
	(begin
	  (build (car items) env)
	  (build-item-list (cdr items))))))

(define-method (build (obj module) env)
  (build-item-list items env))

(define create-cflags
  (lambda (cflags obj env . opt)
    "create a string with compilation flags"
    (let ((str (opt-arg opt 0 "")))
      (if (null? cflags)
	  str
	  (let* ((current (car cflags))
		 (add (cond
		       ((string? current)
			current)
		       ((procedure? current)
			(current obj env))
		       (t ""))))
	    (if (not add)
		add
		(create-cflags (cdr cflags) obj env
			       (string-append str
					      (if (or (string=? "" str)
						      (string=? "" add))
						  ""
						  " ")
					      add))))))))

(define $target-filename
  (lambda (obj env)
    (slot-ref obj 'target-filename)))

(define $source-filename
  (lambda (obj env)
    (slot-ref obj 'source-filename)))
; base )

; c++ (
(define-class c++-file-item (build-file-item))

(define-method (build (obj c++-file-item) env)
  (append-string (assoc 'c++c env) " "
		 (create-cflags (assoc 'c++flags env) obj env)))

(define-method (scan-dependencies (obj c++-file-item) env)
  )
; c++ )

; has-changed timestamp (
(define-method (has-changed? (obj build-file-item) env)
  (< (stat:mtime (stat (slot-ref obj 'target-filename)))
     (stat:mtime (stat (slot-ref obj 'source-filename)))))
; has-changed timestamp )

; misc (
(define opt-arg
  (lambda (args n default)
    "emulates optional arguments in scheme"
    (if (< n (length args))
	(list-ref args n)
	default)))
; misc )

(define-class program-module (module)
  (program-name #:init-value "a.out"))

(define file-item
  (lambda (item source . target)
    (let ((a (make item)))
      (slot-set! a 'source-filename source)
      (if (null? target)
	  (slot-set! a 'target-filename
		     (string-append
		      (substring source 0
				 (string-rindex source #\.)) ".o"))
	  (slot-set! a 'target-filename (car target)))
      a)))
  
(define program
  (lambda (prog-name . pitems)
    (let ((a (make program-module)))
      (slot-set! a 'program-name prog-name)
      (slot-set! a 'items pitems)
      a)))

(define action
  (lambda (actions . items)
    actions))