from pyspark.sql import SparkSession
from pyspark.ml.feature import VectorAssembler
from pyspark.ml.classification import LogisticRegression, DecisionTreeClassifier, RandomForestClassifier
import pyspark.sql.functions as F

import utilsB

LR_MAX_ITER = 1000
LR_REG_PARAM = 0.01
LR_ELASTIC_NET_PARAM = 0.0

DT_MAX_DEPTH = 5

RF_MAX_DEPTH = 12
RF_NUM_TREES = 25


spark = (
    SparkSession.builder.appName("FetalHealthClassification")
    .master("local[*]")
    .getOrCreate()
)

df = spark.read.csv("fetal_health.csv", header=True, inferSchema=True)

print("Original schema:")
df.printSchema()
print(f"Total rows: {df.count()}")

df = df.withColumn(
    "label",
    F.when(F.col("fetal_health") == 1, 0).otherwise(1)
)

print("\nLabel distribution:")
df.groupBy("label").count().show()

feature_cols = [c for c in df.columns if c != "fetal_health" and c != "label"]

assembler = VectorAssembler(inputCols=feature_cols, outputCol="features")
df_features = assembler.transform(df)

train, test = df_features.randomSplit([0.8, 0.2], seed=42)

print("\n" + "="*60)
print("LOGISTIC REGRESSION")
print("="*60)

lr = LogisticRegression(
    labelCol="label",
    featuresCol="features",
    maxIter=LR_MAX_ITER,
    regParam=LR_REG_PARAM,
    elasticNetParam=LR_ELASTIC_NET_PARAM
)
lr_model = lr.fit(train)
lr_predictions = lr_model.transform(test)
lr_accuracy, lr_precision, lr_recall = utilsB.evaluate_model(lr_predictions, "Logistic Regression")

print("\n" + "="*60)
print("DECISION TREE")
print("="*60)

dt = DecisionTreeClassifier(
    labelCol="label",
    featuresCol="features",
    maxDepth=DT_MAX_DEPTH
)
dt_model = dt.fit(train)
dt_predictions = dt_model.transform(test)
dt_accuracy, dt_precision, dt_recall = utilsB.evaluate_model(dt_predictions, "Decision Tree")

print("\n" + "="*60)
print("RANDOM FOREST")
print("="*60)

rf = RandomForestClassifier(
    labelCol="label",
    featuresCol="features",
    maxDepth=RF_MAX_DEPTH,
    numTrees=RF_NUM_TREES
)
rf_model = rf.fit(train)
rf_predictions = rf_model.transform(test)
rf_accuracy, rf_precision, rf_recall = utilsB.evaluate_model(rf_predictions, "Random Forest")

utilsB.print_summary([
    ("Logistic Regression", lr_accuracy, lr_precision, lr_recall),
    ("Decision Tree", dt_accuracy, dt_precision, dt_recall),
    ("Random Forest", rf_accuracy, rf_precision, rf_recall)
])

spark.stop()
