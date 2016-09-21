if(![System.IO.Directory]::Exists([System.IO.Path]::Combine([System.Environment]::CurrentDirectory, "negs"))){
    [System.IO.Directory]::CreateDirectory([System.IO.Path]::Combine([System.Environment]::CurrentDirectory, "negs"))
}
$markfiles = [System.IO.Directory]::GetFiles([System.IO.Path]::Combine([System.Environment]::CurrentDirectory, "marks"), "*", [System.IO.SearchOption]::AllDirectories)
for($n = 0; $n -lt $markfiles.Length; $n++) {
    $sb = New-Object System.Text.StringBuilder
    $sw = New-Object System.IO.StreamWriter(
     [System.IO.Path]::Combine([System.Environment]::CurrentDirectory, "negs", [System.IO.Path]::GetFileNameWithoutExtension($markfiles[$n]) + ".txt"),
     $false, [System.Text.Encoding]::GetEncoding("shift_jis"))
    for($m = 0; $m -lt $markfiles.Length; $m++) {
        if($n -ne $m) {
            $sb.Append("`n")
            $sb.Append($markfiles[$m])
        }
    }
    $sw.Write($sb.ToString().Substring(1))
    $sw.Flush()
    $sw.Close()
    $sw.Dispose()
}