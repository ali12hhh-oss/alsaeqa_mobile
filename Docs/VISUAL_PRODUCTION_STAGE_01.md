# ALSAEQA — المرحلة البصرية الأولى

## الهدف
تحويل اللعبة من الهيكل البرمجي/الـgreybox إلى إنتاج بصري حقيقي قابل للدمج في Unreal Engine 5.8، مع الحفاظ على هوية **الصاعقة** وعدم استخدام مجسمات هندسية بدائية أو شخصيات بألوان مسطحة كحل نهائي.

> مهم: ملفات 3D الثنائية (`.uasset`, `.fbx`, `.mhpkg`, textures, animations) لا يمكن رفعها إلى هذا المستودع بواسطة واجهة GitHub النصية الحالية. لذلك لا أضع ملفات وهمية بأسماء مجسمات حقيقية. هذه المرحلة تثبت مواصفات الأصول ومصادرها وطريقة دمجها، بينما يتم إدخال الملفات الفعلية من Fab/MetaHuman بعد اقتنائها بالحساب صاحب الترخيص.

## معيار القبول البصري
- شخصيات بشرية: skeletal mesh حقيقي + skinning + PBR + شعر/ملابس + LODs.
- البطل: شخصية بشرية أصلية المظهر، ملابس عامل منجم فقير في البداية، ثم تتطور بصريًا مع قوى الرعد دون نسخ تصميم شخصية معروفة.
- الرفيقة: شخصية بشرية أنثى مستقلة بصريًا، ملابس أسيرة/ناجية قابلة للتطور، سلاح وهوية خاصة بها.
- الأعداء: عدو بشري/وحشي قابل لإعادة الاستخدام + أنواع متخصصة، مع skeleton/animations حقيقية.
- الزعماء: silhouette مميز، skeleton مناسب، مراحل قتال مختلفة، مؤثرات مرتبطة بنظام Boss الحالي.
- الثعبان العملاق: creature skeletal mesh حقيقي، رأس/فك/جسم/ذيل قابل للحركة، bite/charge/coil/retreat animations.
- الأسلحة: meshes حقيقية، PBR، sockets، hit traces، ويمكن تغييرها دون إعادة كتابة Combat.
- العالم: تضاريس وبيئات حقيقية، rocks/foliage/ruins/caves/fortress، Nanite حيث يناسب، Lumen حيث يناسب، وLOD/Scalability للأندرويد.
- الأنيميشن: locomotion، idle، start/stop، turns، attacks، hit reactions، death، dodge، abilities، mounting، dismounting، boss phases، snake attacks.

## هوية ALSAEQA الفريدة
الأصول الجاهزة تستخدم **كمواد إنتاج** وليس كشخصيات نهائية كما هي. يجب تعديل الشكل النهائي في Unreal/Blender بحيث يكون لكل شخصية:
1. silhouette خاص.
2. palette ومواد وملابس خاصة بالقصة.
3. علامة/رمز رعد خاص بالصاعقة، غير منقول من لعبة أخرى.
4. اختلاف في الشعر/الوجه/الإكسسوارات/السلاح.
5. عدم استخدام اسم أو شعار أو زي مميز لشخصية معروفة.
6. الاحتفاظ بإثبات الترخيص للأصل المصدر مع المشروع.

## حزمة البطل
**ALSAEQA_Hero**
- Base: MetaHuman أو character pack مرخص ومتوافق UE5.
- تعديل: وجه وشعر وملابس ومعدات أصلية.
- الملابس المبكرة: قماش خشن، جلد، آثار عمل المناجم، حزام أدوات، حقيبة صغيرة، آثار غبار.
- التطور: طبقات حماية وقطع معدنية/رعدية تظهر تدريجيًا مع القصة.
- السلاح الأساسي: سلاح قريب مميز + sockets لأسلحة لاحقة.
- rig: skeleton موحد يسمح بإعادة استخدام locomotion والـcombat animations.

## الرفيقة
**ALSAEQA_Companion**
- Base: MetaHuman/UE5-compatible female character.
- تعديل كامل للوجه والشعر والملابس.
- سلاح خاص بها.
- Animations مستقلة: idle، follow، combat، rescue، capture، reunion، family-clue reactions.
- لا يبدأ بحث العائلة قبل المرحلة 25؛ هذا قيد قصصي وليس قيدًا بصريًا فقط.

## الأعداء
**ALSAEQA_EnemyBase**
- جسم humanoid حقيقي.
- Variants للملابس/الأسلحة/الرؤوس/الدروع.
- Animation set: patrol، alert، chase، light attack، heavy attack، hit، stagger، death، taunt.
- AI الحالي في `AI/` يبقى مسؤولًا عن السلوك، والـAnimBP عن العرض.

## الزعماء
- جسم كبير مميز لكل boss.
- phase-specific animation sets.
- attack telegraphs باستخدام Niagara/Materials، وليس تغيير اللون فقط.
- دعم damage/attack multipliers الموجودة في `ALSAEQABossCharacter` و`ALSAEQABossComponent`.

## الثعبان العملاق
- Skeletal creature حقيقي وليس capsule/primitive.
- jaw + neck + body + tail bones.
- animations: idle, crawl/slither, bite, charge, coil, retreat, death, hit reaction.
- poison presentation عبر Niagara/material، بينما الضرر يبقى في `Status` و`Health`.

## الأسلحة
المطلوب في الحزمة النهائية:
- سلاح البطل الأساسي.
- سلاح الرفيقة.
- sword/scimitar.
- axe.
- spear.
- heavy hammer.
- shield.
- enemy variants.
- boss weapon/unique prop عند الحاجة.

كل سلاح يجب أن يحتوي على PBR maps مناسبة، socket/pivot صحيح، collision/trace strategy، وLOD مناسب.

## البيئات المطلوبة
العالم يجب أن يدعم إعادة زيارة المناطق، لذلك لا نستخدم خريطة واحدة مغلقة.
- Mine / opening mine.
- Deep cave.
- Forest.
- Thunder mountains.
- Monster valley.
- Abandoned city.
- Tyrant fortress.
- Black desert.
- Ice kingdom.
- Giants' land.

يتم دمج الحزم في مناطق متماسكة، مع إعادة تشكيل المواد والإضاءة والـset dressing لتصبح الهوية البصرية خاصة بـALSAEQA.

## الأنيميشن والربط البرمجي
طبقة العرض لا تغيّر منطق اللعب الموجود:
- `Player` يرسل حالات الحركة/الهجوم/القفز/الـthunder charge.
- `Combat` يحدد نافذة الضرر، والأنيميشن يعرضها عبر AnimNotify/State.
- `Companions` يتحكم في حالات الرفيقة، والـAnimBP يعكسها.
- `Boss` يبدل phase animation sets.
- `Creatures` و`GiantSnake` تستخدم skeleton/AnimBP مناسبًا.
- `Mount` يحتاج mount/dismount/riding/sprint animations.
- `Storm` يضيف Niagara/material/audio cues دون ربط الضرر بالـVFX فقط.

## الأداء
الهدف ليس cinematic-only:
- LODs متعددة للشخصيات.
- Optimized MetaHuman pipeline للأجهزة المستهدفة.
- 4K للـmaster عند الحاجة، مع 2K/1K للهواتف.
- Nanite للـstatic meshes التي تستفيد منه.
- تقليل groom/facial fidelity عند Android scalability.
- تجنب تحميل جميع المناطق والأصول الثقيلة دفعة واحدة.

## مصدر الأصول والحقوق
الأولوية للأصول ذات **Fab Standard License** أو ترخيص تجاري واضح. ملخص Fab الرسمي يسمح باستخدام الأصول تجاريًا، تعديلها ودمجها في المشروع وتوزيع المشروع الذي يحتويها، مع منع إعادة بيع الأصل نفسه منفردًا. يجب الاحتفاظ بسجل شراء/ترخيص كل أصل.

مصدر الشخصيات المفضل: MetaHuman داخل Unreal Engine 5.6+؛ وثائق Epic توضح أن MetaHuman Creator مدمج في UE5.6 وأن الشخصيات قابلة للتجهيز والأنيميشن، كما أن أصول MetaHuman المتوافقة متاحة عبر Fab.

## مصادر تم فحصها
- Fab EULA: https://www.fab.com/eula
- Epic MetaHuman Creator: https://dev.epicgames.com/documentation/metahuman/metahuman-creator-in-unreal-engine
- Epic MetaHumans on Fab: https://dev.epicgames.com/documentation/metahuman/metahumans-on-fab
- Epic MetaHuman Assembly: https://dev.epicgames.com/documentation/metahuman/assembly

## أصول مرشحة للفحص قبل الشراء/الإدخال
- Anime-Inspired & MetaHuman Compatible Base Male Character — مناسب كنقطة انطلاق تقنية للبطل، لكن يجب إعادة تصميمه بصريًا وعدم استخدام شكله الافتراضي.
- Modular Fantasy NPC — مفيد لبناء NPCs وأعداء متنوعين، مع إعادة تخصيص كاملة.
- Stylized Medieval Weapons Pack — يحتوي أسلحة PBR متعددة ويدعم UE5.
- Mountain Forest — بيئة غابة/جبال مع Nanite/Lumen/PCG.
- Ultimate Bioluminescent Deep Cave Environment — مناسب للكهوف العميقة.
- Alien & Organic Environment Bundle — مفيد للكهوف والصخور والمناطق غير المألوفة.
- Dragon - Rigged / Medieval Dragon — أمثلة على creature rigging، ولا يعتمد عليها كهوية للعبة إلا بعد تدقيق الترخيص والتوافق.

## شرط الترخيص
لا تعتبر نتيجة البحث وحدها إثباتًا للترخيص. قبل إدخال أي أصل فعلي يجب التأكد من خانة License في صفحة الأصل وقت الاقتناء وحفظ إثبات الشراء/الاكتساب. لا أضع في المستودع أصولًا ثنائية مجهولة الترخيص أو أنسخ أصولًا من ألعاب أخرى.
