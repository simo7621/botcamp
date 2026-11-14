<?php
$mode = $_GET['mode'] ?? 'safe';
$username = $_GET['username'] ?? '';

$pdo = new PDO("sqlite::memory:");
$pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

$pdo->exec("CREATE TABLE users (id INTEGER PRIMARY KEY, username TEXT)");
$pdo->exec("INSERT INTO users (username) VALUES ('admin'), ('alice'), ('bob')");

try {
    if ($mode == "unsafe") {
        $sql = "SELECT * FROM users WHERE username = '$username'";
        $result = $pdo->query($sql);
    } else {
        $stmt = $pdo->prepare("SELECT * FROM users WHERE username = ?");
        $stmt->execute([$username]);
        $result = $stmt;
    }
    echo json_encode($result->fetchAll(PDO::FETCH_ASSOC), JSON_PRETTY_PRINT);
} catch (PDOException $e) {
    echo "Database error";
}
