# Real Unreal Build

المستودع لا يحتوي على نسخة Unreal Engine نفسها، لذلك لا يمكن لـ GitHub-hosted runners العادية بناء المشروع مباشرة.

تمت إضافة workflow حقيقي في `.github/workflows/unreal-build.yml`.

## المتطلبات

يجب توفير GitHub Actions self-hosted runner يعمل على Windows ويحتوي على:

- Unreal Engine 5.8.
- UnrealBuildTool.
- أدوات بناء C++ الخاصة بـ Visual Studio.
- Android SDK/NDK عند إضافة فحص Android فعلي لاحقًا.

ويجب أن تحمل الآلة الوسوم:

`self-hosted`, `windows`, `unreal-5.8`

مع متغير بيئة:

`UE_ROOT`

يشير إلى مجلد تثبيت Unreal Engine 5.8.

## ما الذي يفحصه Workflow؟

1. يتأكد من وجود `ALSAEQA.uproject`.
2. يتأكد أن المشروع مرتبط بـ Unreal Engine 5.8.
3. يبحث عن UnrealBuildTool في تثبيت Unreal المحدد.
4. يشغّل بناء `Win64 Development` فعليًا باستخدام UnrealBuildTool.
5. يفشل الـ workflow صراحةً عند فشل البناء.
6. يرفع Build Receipts إن وُجدت.

## ملاحظة مهمة

وجود ملف workflow **لا يعني أن البناء نجح**. النجاح الحقيقي لا يُعلن إلا بعد تشغيل workflow على runner يحتوي على Unreal Engine 5.8 وظهور نتيجة `success`.
