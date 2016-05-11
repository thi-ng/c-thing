(defproject thi.ng/geom-cljs "0.1.0-SNAPSHOT"
  :description  "thi.ng/geom asm.js"
  :url          "http://thi.ng/geom"

  :min-lein-version "2.5.3"

  :dependencies [[org.clojure/clojure "1.8.0"]
                 [org.clojure/clojurescript "1.8.51"]
                 [thi.ng/geom "0.0.1158-SNAPSHOT"]
                 [thi.ng/domus "0.3.0-SNAPSHOT"]
                 [reagent "0.5.1"]]

  :plugins      [[lein-figwheel "0.5.0-6"]
                 [lein-cljsbuild "1.1.3" :exclusions [[org.clojure/clojure]]]]

  :source-paths ["src"]

  :clean-targets ^{:protect false} ["resources/public/js/compiled" "target"]

  :cljsbuild {:builds
              [{:id           "dev"
                :source-paths ["src"]
                :figwheel     true
                :compiler     {:main                 thi.ng.geom.core
                               :asset-path           "js/compiled/out"
                               :output-to            "resources/public/js/compiled/app.js"
                               :output-dir           "resources/public/js/compiled/out"
                               :source-map-timestamp true}}
               {:id           "min"
                :source-paths ["src"]
                :compiler     {:output-to     "resources/public/js/compiled/app.js"
                               :optimizations :advanced
                               :externs       ["externs.js"]}}]}

  :figwheel {:css-dirs ["resource/public/css"]
             ;; :ring-handler hello_world.server/handler
             })
