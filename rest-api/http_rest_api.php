<?php
header("Content-Type: application/json");

$method = $_SERVER['REQUEST_METHOD'];
$headers = getallheaders();
$input = file_get_contents("php://input");
$data = json_decode($input, true);

if ($input != "" && $data === null) {
    http_response_code(400);
    echo json_encode(["error" => "Invalid JSON"], JSON_PRETTY_PRINT);
    exit;
}

$response = [
    "method" => $method,
    "data" => $data,
    "authorization" => isset($headers['Authorization']) ? "Present" : "Not provided"
];

switch ($method) {
    case "GET":
        http_response_code(200);
        $response["data"] = $_GET;
        break;
    case "POST":
        http_response_code(201);
        break;
    case "PUT":
        http_response_code(200);
        break;
    case "DELETE":
        http_response_code(200);
        break;
    default:
        http_response_code(405);
        $response = ["error" => "Method not allowed"];
}

echo json_encode($response, JSON_PRETTY_PRINT);
