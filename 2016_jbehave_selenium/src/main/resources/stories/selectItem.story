Scenario: Check search in yandex market

Given browser is open and maximized
When navigate to https://yandex.ru
Then page title start with Яндекс
When I click to Market link
Then page title start with Яндекс.Маркет
When I select Electronic-><subsection>->Extended Search section, set min price to <price>, select <providers> as providers and click Apply button
Then there are 10 elements in result
When I remember the item №1, set its name to search panel and press search button
Then result equals to first item

Examples:
|subsection|price|providers|
|Телевизоры|20000|Samsung,LG|
|Наушники|5000|Beats|