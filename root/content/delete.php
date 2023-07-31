<?php
if ($_SERVER["REQUEST_METHOD"] === "POST") {
    $directory = './';

    // Check if the file name is provided
    if (isset($_POST["fileToDelete"]) && !empty($_POST["fileToDelete"])) {
        $fileToDelete = $_POST["fileToDelete"];
        $fullFilePath = $directory . $fileToDelete;

        // Function to safely delete a file
        function deleteFile($filePath) {
            if (file_exists($filePath)) {
                if (unlink($filePath)) {
                    echo "File '$filePath' has been deleted successfully.";
                } else {
                    echo "Error deleting the file '$filePath'.";
                }
            } else {
                echo "File '$filePath' does not exist.";
            }
        }

        deleteFile($fullFilePath);
    } else {
        echo "Please select a file to delete.";
    }
}
?>



<!DOCTYPE html>
<html>
<head>
    <title>Select File to Delete</title>
</head>
<body>
    <h1>Select File to Delete</h1>
    <form action="delete.php" method="post">
        <label for="fileToDelete">Select a file to delete:</label>
        <select name="fileToDelete" id="fileToDelete">
            <?php
            $directory = './';
            $files = scandir($directory);
            foreach ($files as $file) {
                if ($file !== '.' && $file !== '..') {
                    echo '<option value="' . $file . '">' . $file . '</option>';
                }
            }
            ?>
        </select>
        <input type="submit" value="Delete File">
    </form>
</body>
</html>
