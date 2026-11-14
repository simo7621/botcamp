<?php
$message = "";

if ($_SERVER['REQUEST_METHOD'] == 'POST') {
    $file = $_FILES['file'];
    $ext = strtolower(pathinfo($file['name'], PATHINFO_EXTENSION));
    $allowed = ['pdf' => 'application/pdf', 'png' => 'image/png', 'jpg' => 'image/jpeg'];

    if ($file['error'] != 0) {
        $message = "Upload failed";
    } elseif ($file['size'] > 2 * 1024 * 1024) {
        $message = "File is too large";
    } elseif (!isset($allowed[$ext])) {
        $message = "Extension not allowed";
    } elseif (finfo_file(finfo_open(FILEINFO_MIME_TYPE), $file['tmp_name']) != $allowed[$ext]) {
        $message = "File content does not match its extension";
    } else {
        if (!is_dir("uploads")) {
            mkdir("uploads");
        }
        $newName = bin2hex(random_bytes(16)) . "." . $ext;
        move_uploaded_file($file['tmp_name'], "uploads/" . $newName);
        $message = "Uploaded as " . $newName;
    }
}
?>
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>Secure File Upload</title>
</head>
<body>
    <h2>Secure File Upload</h2>
    <p><?= htmlspecialchars($message) ?></p>
    <form method="POST" enctype="multipart/form-data">
        <input type="file" name="file">
        <button type="submit">Upload</button>
    </form>
</body>
</html>
