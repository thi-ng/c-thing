(ns thi.ng.geomc.macros
  (:require
   [cljs.analyzer :as ana]
   [clojure.string :as str]))

(def js-replacements
  {\. \$ \/ \$ \* "_STAR_" \? "_QMARK_" \! "_BANG_" \' "_QUOTE_"})

(defn jsify-symbol
  [sym]
  (->> (str sym)
       (replace js-replacements)
       (apply str)))

(defmacro defproto
  [proto & specs]
  (let [js      'js*
        methods (into {} (map (fn [[f & sigs]] [f (vec sigs)]) specs))
        pname   (-> &env
                    (dissoc :locals)
                    (cljs.analyzer/resolve-var proto)
                    :name
                    jsify-symbol
                    (str "$"))
        proto   (-> proto
                    (vary-meta assoc :protocol-symbol true)
                    (vary-meta assoc-in [:protocol-info :methods] methods)
                    (vary-meta update :jsdoc conj "@interface"))]
    `(do
       (def ~proto (~js "function(){}"))
       ~@(map
          (fn [[f [_ & more :as args]]]
            (let [f    (vary-meta f assoc :protocol proto)
                  fn$  (str pname (jsify-symbol f) "$arity$" (count args))
                  args$ (str/join "," args)]
              `(defn ~f ~args (~js ~(str _ "." fn$ "(" args$ ")")))))
          specs))))
