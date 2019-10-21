<?php
if (isset($_POST['pin']) && isset($_POST['value'])) {
    echo 'Submission received';
    $pin = $_POST['pin'];
    $value = $_POST['value'];
    echo shell_exec('sudo piio writepin ' . $pin . ' ' . $value);
}
?>

<form action="webcontrol.php" method="POST">
    Pin no: <input type="text" name="pin" />
    Pin value: 
    <select name="value">
        <option value="1">1</option>
        <option value="2">2</option>
    </select>

    <input type="submit" name="submit" value="Submit" />
</form>