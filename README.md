# Название
Пишем код большого проекта: быстро, надёжно, удобно

# О чём вообще хочется рассказать
* Интерфейс, внедрение, тесты, реализация
* Проект - это не только прод, но ещё и тесты, отладка и мониторинг
* Нам платят за работающий код
* Принцип единой ответственности упрощает не только тестирование, но и мониторинг и отладку

# План

## Введение
* Всё, что я рассказываю - это мой опыт работы над backend'ом, который работает 24/7 и существует, развивается и поддерживается в течение нескольких лет
* Давайте ответим на вопрос, что такое наш проект?
  * Для этого зададим другой вопрос - за что нам платят деньги? За код, работающий в продакшене.
  * Получается, что наш проект - это код, работающий в продакшене.
  * Но прод должен работать постоянно и не ломаться новыми фичами и релизами.
  * Что мы для этого делаем? Создаём мониторинги и пишем тесты.
  * То есть наш проект - это не только код, который работает в проде, но и код тестов и мониторингов.
  * Дальше. Иногда ломаются тесты или продакшн. Нам надо уметь их отлаживать. Помимо универсальных инструментов иногда приходится создавать утилиты специально для нашего проекта.
  * Итого, наш проект - это не только код, к-й работает в проде, но ещё и код тестов, мониторингов и служебных инструментов.
  * _Здесь идёт красивая картинка с кубиками посередине, который в стороны расползаются к компонентам проекта_

## Знакомство с MCMC
* Когда мы проектируем и программируем, мы должны думать не только о проде, а обо всей картине
* Обычно приходится действовать в условиях неопределённости. По моему опыту, никогда заранее не удаётся предугадать, что понадобится мониторить и какие инструменты тебе понадобятся. Ну и развитие кода продакшена часто может идти не так, как изначально предполагалось.
* Поэтому я всегда стремлюсь держать код в таком состоянии, чтобы было нетрудно:
  * покрыть тестами любой компонент
  * переиспользовать компоненты продакшена в служебной утилите
  * добавить мониторинг в любой компонент системы
* Достигается это за счёт применения принципа "Minimize coupling, maximize cohesion".
* Даем общее описание принципа
  * Отличный пример - зарядки Apple и простые с USB

## Примеры с кодом
* Приводим пример. Делаем класс, в котором будет "всё, что относится к". Потом понимаем, что нам нужен кусок этого класса, а оторвать его мы не можем. Переписываем как надо.
* Показываем картинки: был монолит, стала структура
* Развиваем пример, показывая, что упрощается тестирование и отладка
* Развиваем пример, добавляя мониторинг (показываем на схеме, как мы воткнули новый блок внутрь)
* Показываем пример, где в целях эффективности мы схлопнули несколько блоков
* Вольтрон :)
* Как применять MCMC: SOLID

## Итоги
* Разговор, почему так писать надо сразу (цена внесения изменений, сложность рефакторинга)
* Преимущества: упрощение тестирования, мониторинга, создания отладочных инструментов и повторного использования кода
* Недостатки: более высокий порог входа в систему (частично решается созданием фасадов), discoverability

# Пример
Реализуем сотового оператора. Будем поддерживать множество пользователей и тарифов, которые лежат в базе данных. Пользователи звонят и пополняют счёт.

### Тестирование
* После разговора денег не стало больше
* Разговор длиною в ноль не меняет количество денег на счету

Проблема - нет интерфейса для проверки

### Мониторинг
* Сколько раз звонили по тарифу
* Суммарная длительность звонков по тарифу
* Суммарная длительность звонков абонента
* Количество выходов ноль
* Количество отправленных SMS

### Инструмент
Хотим провести исследование нового тарифа. Берём базу настоящих пользователей, прогоняем их историю на новом тарифе и считаем, сколько звонков ему не удалось совершить.

Проблемы
* нужно грузить только базу пользователей, но код её загрузки жёстко связан с тарифами
* реальным пользователям начнут прилетать SMS

# Полезные ссылки
* http://www.artima.com/designtechniques/cohesion.html