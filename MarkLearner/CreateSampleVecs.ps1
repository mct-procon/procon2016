if(![System.IO.Directory]::Exists([System.IO.Path]::Combine([System.Environment]::CurrentDirectory, "vecs"))){
    [System.IO.Directory]::CreateDirectory([System.IO.Path]::Combine([System.Environment]::CurrentDirectory, "vecs"))
} else {
    foreach($vf in [System.IO.Directory]::GetFiles([System.IO.Path]::Combine([System.Environment]::CurrentDirectory, "vecs"))) {
        [System.IO.File]::Delete($vf)
    }
}
$markfiles = [System.IO.Directory]::GetFiles([System.IO.Path]::Combine([System.Environment]::CurrentDirectory, "marks"), "*", [System.IO.SearchOption]::AllDirectories)
$vecDic = [System.IO.Path]::Combine([System.Environment]::CurrentDirectory, "vecs")
foreach($f in $markfiles) {
    $f_name = [System.IO.Path]::Combine($vecDic, [System.IO.Path]::GetFileNameWithoutExtension($f) + ".vec")
    
    Start-Process -FilePath opencv_createsamples.exe -ArgumentList "-img `"$f`" -vec `"$f_name`" -num 100"
}