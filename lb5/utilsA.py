from pyspark.sql.functions import (
    col,
    avg,
    to_date,
    from_unixtime,
    unix_timestamp,
    sum,
    exists,
    array
)
from pyspark.sql.types import IntegerType, LongType, DoubleType, FloatType, DecimalType


def show_age(df):
    df_with_age = df.withColumn("age", col("year_of_sale") - col("year_built"))
    df_with_age.select("year_built", "year_of_sale", "age").show(5, truncate=False)


def show_avg_sale_date_tax_zip(df):
    df_with_date = df.withColumn(
        "sale_date_parsed", to_date(col("sale_date"), "yyyy-MM-dd")
    )
    df_with_ts = df_with_date.withColumn(
        "sale_ts", unix_timestamp(col("sale_date_parsed"))
    )
    avg_date_df = (
        df_with_ts.groupBy("zip_code", "tax_class")
        .agg(avg("sale_ts").alias("avg_sale_ts"))
        .withColumn(
            "avg_sale_date",
            from_unixtime(col("avg_sale_ts"), "yyyy-MM-dd").cast("date"),
        )
        .drop("avg_sale_ts")
    )
    avg_date_df.show(10, truncate=False)


def show_total_sum_tax_zip(df):
    sum_price_df = df.groupBy("tax_class", "zip_code").agg(
        sum(col("sale_price")).alias("total_sale_price")
    )
    sum_price_df.show(10, truncate=False)


def show_remove_all_zero_rows(df):
    numeric_types = (IntegerType, LongType, DoubleType, FloatType, DecimalType)
    numeric_cols = [
        c for c in df.columns if isinstance(df.schema[c].dataType, numeric_types)
    ]

    identifier_cols = {"_c0", "borough1", "block", "lot"}

    total_rows = df.count()
    columns_with_zeros = []

    for col_name in numeric_cols:
        zero_count = df.filter(col(col_name).isNull() | (col(col_name) == 0)).count()
        zero_ratio = zero_count / total_rows * 100
        if zero_ratio > 0 and zero_ratio < 50.0 and col_name not in identifier_cols:
            columns_with_zeros.append(col_name)

    selected_columns = columns_with_zeros[:10]

    filter_cols = [c for c in selected_columns if c not in identifier_cols]

    df_selected = df.select(selected_columns)

    if len(filter_cols) == 0:
        df_filtered = df_selected
    else:
        all_zero_expr = None
        for c in filter_cols:
            cond = col(c).isNull() | (col(c) == 0)
            if all_zero_expr is None:
                all_zero_expr = cond
            else:
                all_zero_expr = all_zero_expr & cond
        
        df_filtered = df.select(numeric_cols).filter(~all_zero_expr)

    print(f"\nСтрок до удаления: {df_selected.count()}")
    print(f"Строк после удаления: {df_filtered.count()}")
    print(f"Удалено строк: {df_selected.count() - df_filtered.count()}")

    df_filtered.show(5, truncate=False)
