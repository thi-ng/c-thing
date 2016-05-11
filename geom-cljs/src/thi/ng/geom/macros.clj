(ns thi.ng.geom.macros
  (:require
   [cljs.analyzer :as ana]
   [clojure.string :as str]))

(defmacro foo
  [proto module & forms]
  (let [js 'js*]
    `(do
       (~js "~{} = function() { }" ~proto)
       ~@(map
          (fn [[pn cn]]
            `(~js ~(str "~{}.prototype." pn " = function() { return " module "._" cn "(); }") ~proto))
          (partition 2 forms)))))

(defmacro defproto
  [proto & specs]
  (let [js      'js*
        methods (into {} (map (fn [[f & sigs]] [f (vec sigs)]) specs))
        pname   (-> &env
                    (dissoc :locals)
                    (cljs.analyzer/resolve-var proto)
                    :name
                    str
                    (str/replace #"[\./]" "\\$"))
        proto   (-> proto
                    (vary-meta assoc :protocol-symbol true)
                    (vary-meta assoc-in [:protocol-info :methods] methods)
                    (vary-meta update :jsdoc conj "@interface"))]
    `(do
       (def ~proto (~js "function(){}"))
       ~@(map
          (fn [[f [_ & more :as args]]]
            (let [f    (vary-meta f assoc :protocol proto)
                  fn$  (str pname "$" f "$arity$" (count args))
                  args$ (str/join "," args)]
              `(defn ~f ~args (~js ~(str _ "." fn$ "(" args$ ")")))))
          specs))))
