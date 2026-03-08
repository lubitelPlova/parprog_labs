from pyspark.sql import SparkSession

import utilsA


spark = (
    SparkSession.builder.appName("BrooklynSalesAnalysis")
    .master("local[*]")
    .getOrCreate()
)

df = spark.read.csv("brooklyn_sales_map.csv", header=True, inferSchema=True)

print(
    "\n\nЗапрос 1: Добавьте в датасет новую колонку,"
    " содержащую «возраст» жилья.\n\n"
)



utilsA.show_age(df)

print(
    "\n\nЗапрос 2: Выведите таблицу, содержащую среднюю дату продажи для всех"
    " сочетаний индексов (zip_code) и налоговых категорий (tax_class) жилья.\n\n"
)

utilsA.show_avg_sale_date_tax_zip(df)

print(
    "\n\nЗапрос 3:Выведите таблицу, содержащую суммарную стоимость жилья"
    " (sale_price) по всем сочетаниям налоговых категорий (tax_class)"
    " и индексов (zip_code).\n\n"
)

utilsA.show_total_sum_tax_zip(df)

print(
    "\n\nЗапрос 4: Создайте новую таблицу, в которой есть 10 колонок"
    " исходного датафрейма, в которых нулевые значения есть, но"
    " не преобладают. После этого удалите все строки, в которых"
    " содержатся исключительно нули\n\n"
)

utilsA.show_remove_all_zero_rows(df)

spark.stop()
