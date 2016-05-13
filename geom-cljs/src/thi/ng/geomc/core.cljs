(ns thi.ng.geomc.core
  (:require-macros
   [thi.ng.geomc.macros :refer [defproto]])
  (:require
   [thi.ng.math.core :as m]
   [thi.ng.geom.core :as g]
   [thi.ng.geom.vector :as v]))

(defonce geom (js/geom))

(enable-console-print!)

(defproto IFoo
  (mag [_])
  (add [_ v])
  (cross [_ v])
  (mix [_ b t]))

(deftype Vec2f [ptr]
  IFoo
  (mag [_] (._ct_mag2f js/geom ptr))
  (add [_ v] (._ct_add2fv_imm js/geom ptr (.-ptr ^Vec2f v)) _))

(deftype Vec3f [ptr]
  IFoo
  (mag [_] (._ct_mag3f js/geom ptr))
  (add [_ v] (._ct_add3fv_imm js/geom ptr (.-ptr ^Vec3f v)) _)
  (cross [_ v] (._ct_cross3fv_imm js/geom ptr (.-ptr ^Vec3f v)) _)
  (mix [_ b t] (._ct_mix3fv_imm js/geom ptr (.-ptr ^Vec3f b) t) _))

(defn ^:export vec2f
  [x y] (Vec2f. (._ct_vec2f js/geom x y nil)))

(defn ^:export vec3f
  [x y z] (Vec3f. (._ct_vec3f js/geom x y z nil)))

(defn ^:export main
  []
  (let [a2 (add (vec2f 10 10) (vec2f 90 90))
        b2 (add (vec2f 100 100) (vec2f 100 100))
        a3 (add (vec3f 10 10 10) (vec3f 90 90 90))
        b3 (add (vec3f 100 100 100) (vec3f 100 100 100))]
    (clj->js [(mag a2) (mag b2) (mag a3) (mag b3)])))

(defn ^:export bench-cross
  []
  (let [a (vec3f 1 0 0)
        b (vec3f 0 1 0)
        t0 (.getTime (js/Date.))]
    (loop [i 1e6]
      (when (pos? i)
        (cross a b)
        (recur (dec i))))
    (prn (- (.getTime (js/Date.)) t0) (.-ptr a))))

(defn ^:export bench-cross2
  []
  (let [a (v/vec3 1 0 0)
        b (v/vec3 0 1 0)
        t0 (.getTime (js/Date.))]
    (loop [i 1e6]
      (when (pos? i)
        (m/cross a b)
        (recur (dec i))))
    (prn (- (.getTime (js/Date.)) t0) (.-ptr a))))

(defn ^:export bench-mix
  []
  (let [a (vec3f 1 0 0)
        b (vec3f 0 1 0)
        t0 (.getTime (js/Date.))]
    (loop [i 1e6]
      (when (pos? i)
        (mix a b 0.5)
        (recur (dec i))))
    (prn (- (.getTime (js/Date.)) t0) (.-ptr a))))

(defn ^:export bench-mix2
  []
  (let [a (v/vec3 1 0 0)
        b (v/vec3 0 1 0)
        t0 (.getTime (js/Date.))]
    (loop [i 1e6]
      (when (pos? i)
        (m/mix a b 0.5)
        (recur (dec i))))
    (prn (- (.getTime (js/Date.)) t0) (.-ptr a))))
