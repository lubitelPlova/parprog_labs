from pyspark.ml.evaluation import MulticlassClassificationEvaluator


def create_evaluators(label_col="label", prediction_col="prediction"):
    evaluator_acc = MulticlassClassificationEvaluator(
        labelCol=label_col, predictionCol=prediction_col, metricName="accuracy"
    )
    evaluator_prec = MulticlassClassificationEvaluator(
        labelCol=label_col, predictionCol=prediction_col, metricName="weightedPrecision"
    )
    evaluator_rec = MulticlassClassificationEvaluator(
        labelCol=label_col, predictionCol=prediction_col, metricName="weightedRecall"
    )
    return evaluator_acc, evaluator_prec, evaluator_rec


def evaluate_model(predictions, model_name, label_col="label", prediction_col="prediction"):
    evaluator_acc, evaluator_prec, evaluator_rec = create_evaluators(label_col, prediction_col)

    accuracy = evaluator_acc.evaluate(predictions)
    precision = evaluator_prec.evaluate(predictions)
    recall = evaluator_rec.evaluate(predictions)

    print(f"\n{'='*50}")
    print(f"Model: {model_name}")
    print(f"{'='*50}")
    print(f"Accuracy:  {accuracy:.4f}")
    print(f"Precision: {precision:.4f}")
    print(f"Recall:    {recall:.4f}")

    print("\nConfusion Matrix:")
    print("Predicted ->")
    print("         |  0  |  1  ")
    print("-" * 25)

    conf_matrix = predictions.groupBy(label_col, prediction_col).count().collect()
    matrix = [[0, 0], [0, 0]]
    for row in conf_matrix:
        label = int(row[label_col])
        pred = int(row[prediction_col])
        count = row["count"]
        matrix[label][pred] = count

    for i, row in enumerate(matrix):
        print(f"Actual {i} | {row[0]:3d} | {row[1]:3d}")

    tn = matrix[0][0]
    fp = matrix[0][1]
    fn = matrix[1][0]
    tp = matrix[1][1]

    print(f"\nTrue Positives (TP):  {tp}")
    print(f"True Negatives (TN): {tn}")
    print(f"False Positives (FP): {fp}")
    print(f"False Negatives (FN): {fn}")

    predictions.groupBy(label_col, prediction_col).count().show()

    return accuracy, precision, recall


def print_summary(results):
    print("\n" + "="*60)
    print("SUMMARY")
    print("="*60)
    print(f"{'Model':<25} {'Accuracy':<12} {'Precision':<12} {'Recall':<12}")
    print("-"*60)
    for model_name, accuracy, precision, recall in results:
        print(f"{model_name:<25} {accuracy:<12.4f} {precision:<12.4f} {recall:<12.4f}")
