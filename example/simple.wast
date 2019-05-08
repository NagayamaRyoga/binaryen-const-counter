
(module
    (export "add3" (func $add3))
    (func $add3 (param i32) (result i32)
        (i32.add
            (local.get 0)
            (i32.const 3)))
)
