# PGO для Android libquickjs.so

`quickjs-android.profdata` — профиль clang, по которому собирается релизный `.so`
(`-fprofile-use`, см. `QUICKJS_PGO` в `../CMakeLists.txt`). Снят 2026-09-09 на Redmi Note 8 Pro
(arm64): старт гостя Eskhata, ввод, клики, скролл lazyColumn/lazyRow. Синтетический зонд
интерпретатора на устройстве: 275 мс → 232 мс (−16 %).

Переснимать после заметных правок гостя или обновления QuickJS — устаревший профиль не ломает
сборку, но и не помогает (предупреждения о несовпадении погашены).

## Как переснять

```shell
# 1. Инструментированный .so; профиль пишет common/quickjs_pgo.c раз в 20 с
./gradlew :composeApp:installDebug -PquickjsPgo=generate

# 2. Погонять приложение на телефоне ≥ 30 с на каждом сценарии, потом забрать профили
#    (нужна debug-сборка — run-as)
PKG=com.ilhom.nativecmpweb
for f in $(adb shell run-as $PKG ls files | grep profraw); do
  adb exec-out run-as $PKG cat files/$f > /tmp/$f
done

# 3. Слить llvm-profdata из того же NDK, которым собирается .so
NDK=~/Library/Android/sdk/ndk/<версия>
$NDK/toolchains/llvm/prebuilt/darwin-x86_64/bin/llvm-profdata merge \
    -o quickjs-android.profdata /tmp/quickjs-*.profraw

# 4. Обычная сборка подхватит профиль сама (умолчание -PquickjsPgo=use)
./gradlew :composeApp:installDebug
```

Сборка без профиля: `-PquickjsPgo=off`.
