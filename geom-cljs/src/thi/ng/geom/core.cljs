(ns thi.ng.geom.core
  (:require-macros [thi.ng.geom.macros :refer [defproto]]))

(defonce geom (js/geom))

(enable-console-print!)

(defproto IFoo
  (mag [_])
  (addxy [_ x y]))

(deftype Vec2f [ptr]
  IFoo
  (mag [_] (._ct_mag2f js/geom ptr))
  (addxy [_ x y] (._ct_add2fxy_imm js/geom ptr x y) _))

(defn ^:export vec2f
  [x y] (Vec2f. (._ct_vec2f js/geom x y nil)))

(defn ^:export main
  []
  (let [a (addxy (vec2f 10 10) 90 90)
        b (addxy (vec2f 100 100) 100 100)]
    (clj->js [(mag a) (mag b)])))
