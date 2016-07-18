(ns thi.ng.geomc.core
  (:require-macros
   [thi.ng.geomc.macros :refer [defproto]])
  (:require
   [thi.ng.math.core :as m]
   [thi.ng.geom.core :as g]
   [thi.ng.geom.circle :as c]
   [thi.ng.geom.vector :as v]
   [thi.ng.strf.core :as f]))

(defonce cthing (js/cthing))

(enable-console-print!)

(defproto IMag
  (mag [_]))

(defproto IMathOps
  (+! [_ v])
  (-! [_ v])
  (*! [_ v])
  (div! [_ v]))

(defproto ICross
  (cross! [_ v]))

(defproto IMix
  (mix! [_ v t]))

(defproto IArea
  (area [_]))

(defproto ITessellate
  (tessellate [_ opts]))

(deftype Vec2f [ptr]
  IMag
  (mag [_] (._ct_mag2f js/cthing ptr))
  IMathOps
  (+! [_ v] (._ct_add2fv_imm js/cthing ptr (.-ptr ^Vec2f v)) _)
  (-! [_ v] (._ct_sub2fv_imm js/cthing ptr (.-ptr ^Vec2f v)) _)
  IPrintWithWriter
  (-pr-writer [_ writer opts]
    (pr-sequential-writer
     writer pr-writer "#vec2f [" " " "]" opts
     [(aget (.-HEAPF32 js/cthing) (unsigned-bit-shift-right ptr 2))
      (aget (.-HEAPF32 js/cthing) (unsigned-bit-shift-right (+ ptr 4) 2))])))

(deftype Vec3f [ptr]
  IMag
  (mag [_] (._ct_mag3f js/cthing ptr))
  IMathOps
  (+! [_ v] (._ct_add3fv_imm js/cthing ptr (.-ptr ^Vec3f v)) _)
  ICross
  (cross! [_ v] (._ct_cross3fv_imm js/cthing ptr (.-ptr ^Vec3f v)) _)
  IMix
  (mix! [_ b t] (._ct_mix3fv_imm js/cthing ptr (.-ptr ^Vec3f b) t) _)
  IPrintWithWriter
  (-pr-writer [_ writer opts]
    (pr-sequential-writer
     writer pr-writer "#vec3f [" " " "]" opts
     [(aget (.-HEAPF32 js/cthing) (unsigned-bit-shift-right ptr 2))
      (aget (.-HEAPF32 js/cthing) (unsigned-bit-shift-right (+ ptr 4) 2))
      (aget (.-HEAPF32 js/cthing) (unsigned-bit-shift-right (+ ptr 8) 2))])))

(deftype Circle2f [ptr]
  IArea
  (area [_] (._ct_circle2f_area js/cthing ptr))
  ITessellate
  (tessellate [_ {:keys [res target]}]
    (let [target (or target (._malloc js/cthing (* (inc res) 36)))]
      (._ct_circle2f_tessellate js/cthing ptr target res)))
  IPrintWithWriter
  (-pr-writer [_ writer opts]
    (pr-sequential-writer
     writer pr-writer "#circle2f [" " " "]" opts
     [(aget (.-HEAPF32 js/cthing) (unsigned-bit-shift-right ptr 2))
      (aget (.-HEAPF32 js/cthing) (unsigned-bit-shift-right (+ ptr 4) 2))
      (aget (.-HEAPF32 js/cthing) (unsigned-bit-shift-right (+ ptr 8) 2))])))

(defn ^:export vec2f
  [x y] (Vec2f. (._ct_vec2f js/cthing x y nil)))

(defn ^:export vec3f
  [x y z] (Vec3f. (._ct_vec3f js/cthing x y z nil)))

(defn ^:export circle2f
  [x y r]
  (let [ptr (._malloc js/cthing 12)]
    (Circle2f. (._ct_circle2f_init js/cthing ptr x y r))))

(defn ^:export main
  []
  (let [a2 (+! (vec2f 10 10) (vec2f 90 90))
        b2 (+! (vec2f 100 100) (vec2f 100 100))
        a3 (+! (vec3f 10 10 10) (vec3f 90 90 90))
        b3 (+! (vec3f 100 100 100) (vec3f 100 100 100))]
    (clj->js [(mag a2) (mag b2) (mag a3) (mag b3)])))

(defn ^:export bench-cross
  []
  (let [a (vec3f 1 0 0)
        b (vec3f 0 1 0)
        t0 (.getTime (js/Date.))]
    (loop [i 1e6]
      (when (pos? i)
        (cross! a b)
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
        (mix! a b 0.5)
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

(defn ^:export bench-tessel
  []
  (let [c (circle2f 0 0 1)
        tptr (._malloc js/cthing (* 81 3 2 4))
        t0 (.getTime (js/Date.))]
    (loop [i 1e5]
      (when (pos? i)
        (tessellate c {:res 80 :target tptr})
        (recur (dec i))))
    (._free js/cthing tptr)
    (prn (- (.getTime (js/Date.)) t0))))

(defn ^:export bench-tessel2
  []
  (let [c (c/circle 0 0 1)
        t0 (.getTime (js/Date.))]
    (loop [i 1e5]
      (when (pos? i)
        (g/tessellate c {:res 80})
        (recur (dec i))))
    (prn (- (.getTime (js/Date.)) t0))))
