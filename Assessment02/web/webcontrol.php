<?php
// Read pin
if (isset($_POST['readpin_submit'])) {
    if (!isset($_POST['pin'])) {
        echo 'ERROR: Incorrect POST parameters specified.<br />';
        return;
    }

    $pin = clean_string($_POST['pin']);

    echo 'OUTPUT: ';
    echo nl2br(shell_exec('sudo piio readpin ' . $pin));
    echo '<br />';
}

// Write pin
if (isset($_POST['writepin_submit'])) {
    if (!isset($_POST['pin']) || !isset($_POST['value'])) {
        echo 'ERROR: Incorrect POST parameters specified.<br />';
        return;
    }

    $pin = clean_string($_POST['pin']);
    $value = clean_string($_POST['value']);

    echo 'OUTPUT: ';
    echo nl2br(shell_exec('sudo piio writepin ' . $pin . ' ' . $value));
    echo '<br />';
}

// Toggle pin
if (isset($_POST['togglepin_submit'])) {
    if (!isset($_POST['pin']) || !isset($_POST['times']) || !isset($_POST['ms'])) {
        echo 'ERROR: Incorrect POST parameters specified.<br />';
        return;
    }

    $pin = clean_string($_POST['pin']);
    $times = clean_string($_POST['times']);
    $ms = clean_string($_POST['ms']);

    echo 'OUTPUT: ';
    echo nl2br(shell_exec('sudo piio toggle ' . $pin . ' ' . $times . ' ' . $ms));
    echo '<br />';
}

function clean_string($string) {
    return preg_replace('/[^\w]/', '', $string);
}

?>

<h4>Read Pin</h4>
<form action="webcontrol.php" method="POST">
    Pin no: <input type="text" name="pin" />

    <input type="submit" name="readpin_submit" value="Submit" />
</form>

<br />

<h4>Write Pin</h4>
<form action="webcontrol.php" method="POST">
    Pin no: <input type="text" name="pin" />
    Pin value: 
    <select name="value">
        <option value="0">0</option>
        <option value="1">1</option>
    </select>

    <input type="submit" name="writepin_submit" value="Submit" />
</form>

<br />

<h4>Toggle Pin</h4>
<form action="webcontrol.php" method="POST">
    Pin no: <input type="text" name="pin" />
    Times: <input type="number" name="times" />
    Ms: <input type="number" name="ms" />

    <input type="submit" name="togglepin_submit" value="Submit" />
</form>