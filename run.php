<!DOCTYPE html>
<html>
<head>
	<title>Running Log</title>
	<meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no" />
</head>
<body>
<h1>
<ul>
	<?php
		$lastrun = fopen("lastrun.txt", "r");
		$score = fgets($lastrun);
		echo "<li>Score: " . $score . "</li>";
	?>
	<li><a href="incrementRun.php">Increment</a></li>
	<li><a href="decrementRun.php">Decrement</a></li>
</ul>
</h1>
</body>
</html>