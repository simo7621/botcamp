<?php
session_set_cookie_params([
    'lifetime' => 0,
    'path' => '/',
    'secure' => !empty($_SERVER['HTTPS']),
    'httponly' => true,
    'samesite' => 'Strict'
]);
session_start();

if (isset($_POST['login'])) {
    session_regenerate_id(true);
    $_SESSION['user'] = 'Student';
}

if (isset($_POST['logout'])) {
    $_SESSION = [];
    session_destroy();
}

setcookie('protected_cookie', 'value1', [
    'path' => '/',
    'httponly' => true,
    'samesite' => 'Strict'
]);

setcookie('unprotected_cookie', 'value2', [
    'path' => '/',
    'httponly' => false
]);
?>
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Secure Session and Cookies</title>
</head>
<body>
    <h2>Secure Session and Cookies</h2>
    <p>User: <?= isset($_SESSION['user']) ? htmlspecialchars($_SESSION['user']) : "Not logged in" ?></p>
    <form method="POST">
        <button name="login">Login</button>
        <button name="logout">Logout</button>
    </form>
    <p>Cookies visible to JavaScript:</p>
    <pre id="cookies"></pre>
    <script>
        document.getElementById("cookies").textContent = document.cookie;
    </script>
</body>
</html>
